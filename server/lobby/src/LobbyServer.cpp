/**
 * @file server/lobby/src/LobbyServer.cpp
 * @ingroup lobby
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Lobby server class.
 *
 * This file is part of the Lobby Server (lobby).
 *
 * Copyright (C) 2012-2016 COMP_hack Team <compomega@tutanota.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "LobbyServer.h"

// lobby Includes
#include "LobbyConnection.h"

// libcomp Includes
#include <Log.h>
#include <ManagerPacket.h>

// Object Includes
#include "LobbyConfig.h"

using namespace lobby;

LobbyServer::LobbyServer(const libcomp::String& listenAddress, uint16_t port) :
    libcomp::BaseServer(listenAddress, port)
{
    objects::LobbyConfig config;
    ReadConfig(&config, "lobby.xml");

    // Add the managers to the worker.
    mWorker.AddManager(std::shared_ptr<libcomp::Manager>(new ManagerPacket()));

    // Start the worker.
    mWorker.Start();
}

LobbyServer::~LobbyServer()
{
}

std::shared_ptr<libcomp::TcpConnection> LobbyServer::CreateConnection(
    asio::ip::tcp::socket& socket)
{
    auto connection = std::shared_ptr<libcomp::TcpConnection>(
        new libcomp::LobbyConnection(socket, CopyDiffieHellman(
            GetDiffieHellman())
        )
    );

    // Assign this to the only worker available.
    std::dynamic_pointer_cast<libcomp::LobbyConnection>(
        connection)->SetMessageQueue(mWorker.GetMessageQueue());

    // Make sure this is called after connecting.
    connection->SetSelf(connection);
    connection->ConnectionSuccess();

    return connection;
}
