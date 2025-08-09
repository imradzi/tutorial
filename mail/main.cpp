/*

smtps_simple_msg.cpp
--------------------

Connects to SMTP server via START_TLS and sends a simple message.


Copyright (C) 2016, Tomislav Karastojkovic (http://www.alepho.com).

Distributed under the FreeBSD license, see the accompanying file LICENSE or
copy at http://www.freebsd.org/copyright/freebsd-license.html.

*/

#include <iostream>
#include <mailio/message.hpp>
#include <mailio/smtp.hpp>

using mailio::dialog_error;
using mailio::mail_address;
using mailio::message;
using mailio::smtp_error;
using mailio::smtps;
using std::cout;
using std::endl;

int main() {
  try {
    // create mail message
    message msg;
    msg.from(mail_address(
        "PharmaPOS admin",
        "admin@pharmapos.com")); // set the correct sender name and address
    msg.add_recipient(mail_address(
        "user: Radzi",
        "imradzi@gmail.com")); // set the correct recipent name and address
    msg.subject("smtps simple message");
    msg.content("Hello, World!");

    std::cout << "Connecting to server" << std::endl;
    // connect to server
    smtps conn("pharmapos.com", 25); // 587);
    // modify username/password to use real credentials
    std::cout << "Authenticate server" << std::endl;
    conn.authenticate("mailer", "mailer1234", smtps::auth_method_t::LOGIN);
    std::cout << "submitting" << std::endl;
    conn.submit(msg);
    std::cout << "OK." << std::endl;
  } catch (smtp_error &exc) {
    cout << exc.what() << endl;
  } catch (dialog_error &exc) {
    cout << exc.what() << endl;
  }

  return EXIT_SUCCESS;
}
