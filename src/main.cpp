//============================================================================
//
//
//
//============================================================================



#include <stdio.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <ui/GLViewer.h>
#include <gltools/gl.h>
#include <gltools/misc.h>
#include <util/StringManip.h>
#include <util/Path.h>
#include <gfx/Geometry.h>
#include <gfx/ObjIO.h>
#include <gfx/Shader.h>
#include <gfx/Texture.h>
#include <gfx/Image.h>
#include <gfx/Context.h>
#include <gfx/FCurve.h>
#include <gfx/glsl/common.h>
#include <gfx/FBO.h>

#include <tinythread/tinythread.h>



#include <3rdparty\ip\TcpSocket.h>

base::GLViewer *glviewer;
base::ContextPtr context;





void render( base::CameraPtr cam )
{
}






void init()
{
	std::cout << "init!\n";


	GLenum glewResult = glewInit();
	if (GLEW_OK != glewResult)
	{
		std::cout << "glew init failed\n";
	}

	// connect to composer and retrieve scene
}

void shutdown()
{
}

struct RemoteApplication
{
	bool                    m_isConnected;
	TcpSocket                    m_server;
	TcpSocket                    m_client;
	tthread::thread    *m_listeningThread;

	RemoteApplication() : m_isConnected(false)
	{
		// start server thread
		m_listeningThread = new tthread::thread(server, this);

	}

	~RemoteApplication()
	{
		delete m_listeningThread;
		m_listeningThread = 0;
	}

	// play
	// stop
	// setTime();
	// load( std::string operatorPath, BSONPtr data )
	void loadOperatorGraph( /*BSONPtr operatorGraph*/ )
	{
		if( !m_isConnected )
			return;

		// send command to client
		std::cout << "sending test message..." << std::endl;
		std::string test = "huhu";
		m_client.Send(test.c_str(), test.size());

		// receive response
		//char buffer[1000];
		//std::cout << "sending test message..." << std::endl;
		//int numBytesReceived = client.Receive( buffer, 10000000 );
		//std::cout << numBytesReceived << std::endl;
	}




	static void server( void *args )
	{
		RemoteApplication *_this = (RemoteApplication *)args;
		std::cout << "listening for client..." << std::endl;
		_this->m_server.Bind( IpEndpointName(12345) );
		_this->m_server.Listen();

		 _this->m_server.Accept( _this->m_client );

		if( _this->m_client.isValid() )
		{
			std::cout << "connected to client..." << std::endl;
			_this->m_isConnected = true;

			//_this->loadOperatorGraph();

			// sleep until we have something to do
			/*

			// sending test message
			std::cout << "sending test message..." << std::endl;
			std::string test = "huhu";
			client.Send(test.c_str(), test.size());

			// receive response
			char buffer[1000];
			std::cout << "sending test message..." << std::endl;
			int numBytesReceived = client.Receive( buffer, 10000000 );
			std::cout << numBytesReceived << std::endl;
			*/

			//SOCKET_ERROR
		}

		std::cout << "closing thread" << std::endl;
	}
};






int main(int argc, char ** argv)
{
	RemoteApplication remoteApp;

	base::Application app;
	glviewer = new base::GLViewer( 800, 600, "demo" );
	glviewer->show();
	return app.exec();
}
