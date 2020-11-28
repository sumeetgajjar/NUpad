//
// Created by madhurjain on 11/28/20.
//
#include <glog/logging.h>
#include "app_websocket.h"

namespace websocket {

// Define a callback to handle incoming messages
void on_message(server *s, websocketpp::connection_hdl hdl, message_ptr msg) {
  LOG(INFO) << "on_message called with hdl: " << hdl.lock().get()
            << " and message: " << msg->get_payload();

  // check for a special command to instruct the server to stop listening so
  // it can be cleanly exited.
  if (msg->get_payload() == "stop-listening") {
    s->stop_listening();
    return;
  }

  try {
    s->send(hdl, msg->get_payload(), msg->get_opcode());
  } catch (websocketpp::exception const &e) {
    LOG(INFO) << "Echo failed because: (" << e.what() << ")";
  }
}


void run_websocket_thread() {
  // Create a server endpoint
  server echo_server;

  try {
    // Set logging settings
    echo_server.set_access_channels(websocketpp::log::alevel::all);
    echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

    // Initialize Asio
    echo_server.init_asio();

    // Register our message handler
    echo_server.set_message_handler(bind(&on_message, &echo_server,
                                         websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

    // Listen on port 9002
    echo_server.listen(9002);

    // Start the server accept loop
    echo_server.start_accept();

    // Start the ASIO io_service run loop
    echo_server.run();
  } catch (websocketpp::exception const &e) {
    LOG(INFO) << e.what();
  } catch (...) {
    LOG(INFO) << "other exception";
  }
}

} // namespace websocket
