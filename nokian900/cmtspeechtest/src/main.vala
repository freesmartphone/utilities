/*
 * (C) 2011 Michael 'Mickey' Lauer <mlauer@vanille-media.de>
 */

MainLoop loop;
IOChannel channel;
CmtSpeech.Connection connection;

public static void handleDataEvent()
{
    debug( @"handleDataEvent during protocol state $(connection.protocol_state())" );

    var dlbuf = CmtSpeech.FrameBuffer();
    var ulbuf = CmtSpeech.FrameBuffer();

    var ok = connection.dl_buffer_acquire( out dlbuf );
    if ( ok == 0 )
    {
        debug( "received DL packet w/ %u bytes", dlbuf.count );
        if ( connection.protocol_state() == CmtSpeech.State.ACTIVE_DLUL )
        {
            debug( "protocol state is ACTIVE_DLUL, uploading as well..." );
            ok = connection.ul_buffer_acquire( out ulbuf );
            if ( ulbuf.pcount == dlbuf.pcount )
            {
                debug( "looping DL packet to UL with %u payload bytes", dlbuf.pcount );
                Memory.copy( ulbuf.payload, dlbuf.payload, dlbuf.pcount );
            }
            connection.ul_buffer_release( ulbuf );
        }
        connection.dl_buffer_release( dlbuf );
    }
}

public static void handleControlEvent()
{
    debug( @"handleControlEvent during protocol state $(connection.protocol_state())" );

    CmtSpeech.Event event = CmtSpeech.Event();
    CmtSpeech.Transition transition = 0;

    connection.read_event( event );

    debug( @"read event, type is $(event.msg_type)" );
    transition = connection.event_to_state_transition( event );

    switch( transition )
    {
        case CmtSpeech.Transition.INVALID:
          debug( "ERROR: invalid state transition");
          break;

        case CmtSpeech.Transition.1_CONNECTED:
        case CmtSpeech.Transition.2_DISCONNECTED:
        case CmtSpeech.Transition.3_DL_START:
        case CmtSpeech.Transition.4_DLUL_STOP:
        case CmtSpeech.Transition.5_PARAM_UPDATE:
          debug( @"state transition ok, new state is $transition" );
          break;

        case CmtSpeech.Transition.6_TIMING_UPDATE:
        case CmtSpeech.Transition.7_TIMING_UPDATE:
          debug( "WARNING: modem UL timing update ignored" );
          break;

        case CmtSpeech.Transition.10_RESET:
        case CmtSpeech.Transition.11_UL_STOP:
        case CmtSpeech.Transition.12_UL_START:
          debug( @"state transition ok, new state is $transition" );
          break;

        default:
          assert_not_reached();
    }
}

public static bool onTimeout()
{
    if ( connection != null )
    {
        var ok = connection.state_change_call_status( true );
        debug( @"change call state returned: $ok" );
    }

    return false; // don't call again
}

public static bool onInputFromChannel( IOChannel source, IOCondition condition )
{
    debug( "onInputFromChannel, condition = %d", condition );

    assert( condition == IOCondition.HUP || condition == IOCondition.IN );

    if ( condition == IOCondition.HUP )
    {
        debug( "HUP, closing" );
        loop.quit();
        return false;
    }

	CmtSpeech.EventType flags = 0;
    var ok = connection.check_pending( out flags );
    if ( ok < 0 )
    {
        debug( "error while checking for pending events..." );
    }
    else if ( ok == 0 )
    {
        debug( "D'oh, cmt speech readable, but no events pending..." );
    }
    else
    {
        debug( "connection reports pending events with flags 0x%0X", flags );

        if ( ( flags & CmtSpeech.EventType.DL_DATA ) == CmtSpeech.EventType.DL_DATA )
        {
            handleDataEvent();
        }
        else if ( ( flags & CmtSpeech.EventType.CONTROL ) == CmtSpeech.EventType.CONTROL )
        {
            handleControlEvent();
        }
        else
        {
            debug( "event no DL_DATA nor CONTROL, ignoring" );
        }
    }

    return true;
}

public static void main()
{
    debug( "initializing cmtspeed" );
    CmtSpeech.init();

    debug( "setting up traces" );
    CmtSpeech.trace_toggle( CmtSpeech.TraceType.STATE_CHANGE, true );
    CmtSpeech.trace_toggle( CmtSpeech.TraceType.IO, true );
    CmtSpeech.trace_toggle( CmtSpeech.TraceType.DEBUG, true );

    debug( "instanciating connection" );
    connection = new CmtSpeech.Connection();
    if ( connection == null )
    {
        error( "Can't instanciate connection" );
    }

    var fd = connection.descriptor();
    if ( fd == -1 )
    {
        error( "File descriptor invalid" );
    }

    debug( "creating channel and mainloop" );
    loop = new MainLoop();

    channel = new IOChannel.unix_new( fd );
    channel.add_watch( IOCondition.IN | IOCondition.HUP, onInputFromChannel );

    Timeout.add_seconds( 3, onTimeout );

    debug( "--> loop" );
    loop.run();
    debug( "<-- loop" );
}

