/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// Copyright (c) 1996-2018, Live Networks, Inc.  All rights reserved
// A test program that demonstrates how to stream - via unicast RTP
// - various kinds of file on demand, using a built-in RTSP server.
// main program

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"

namespace {
static UsageEnvironment* s_env;

// To make the second and subsequent client for each stream reuse the same
// input stream as the first client (rather than playing the file from the
// start for each client), change the following "False" to "True":
static Boolean reuseFirstSource = True;
}
static void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms,
               char const* streamName, char const* inputFileName); // fwd

static char newDemuxWatchVariable;


void run_on_demand_server() {
  // Begin by setting up our usage environment:
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  s_env = BasicUsageEnvironment::createNew(*scheduler);
  // Create the RTSP server:
  RTSPServer* rtspServer = RTSPServer::createNew(*s_env, 8554, nullptr);
  if (rtspServer == NULL) {
    *s_env << "Failed to create RTSP server: " << s_env->getResultMsg() << "\n";
    return;
  }

    char const* descriptionString
    = "Session streamed by \"testOnDemandRTSPServer\"";

    char const* streamName = "mirror";
    ServerMediaSession* sms
    = ServerMediaSession::createNew(*s_env, streamName, streamName,
                    descriptionString);

    sms->addSubsession(H264VideoFileServerMediaSubsession
            ::createNew(*s_env, "test.264", true));
    sms->addSubsession(ADTSAudioFileServerMediaSubsession
            ::createNew(*s_env, "test.aac", true));
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, "mirror", "");


  // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
  // Try first with the default HTTP port (80), and then with the alternative HTTP
  // port numbers (8000 and 8080).

  if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
    *s_env << "\n(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling.)\n";
  } else {
    *s_env << "\n(RTSP-over-HTTP tunneling is not available.)\n";
  }

  s_env->taskScheduler().doEventLoop(); // does not return
}

static void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms,
               char const* streamName, char const* inputFileName) {
  char* url = rtspServer->rtspURL(sms);
  UsageEnvironment& env = rtspServer->envir();
  env << "\n\"" << streamName << "\" stream, from the file \""
      << inputFileName << "\"\n";
  env << "Play this stream using the URL \"" << url << "\"\n";
  delete[] url;
}
