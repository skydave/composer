//============================================================================
//
//
//
//============================================================================

#include <QtGui>
#include <QApplication>

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

struct Application
{
	// global application scope operator graph ptr etc.
	// render function will access this

	virtual void loadOperatorGraph( /*BSONPtr operatorGraph*/ )
	{

	}

	virtual void play()
	{
	}

	virtual void stop()
	{
	}
	virtual void setTime()
	{
	}
};

struct RemoteApplication : public Application
{
	bool                    m_isConnected;
	TcpSocket                    m_server;
	TcpSocket                    m_client;
	tthread::thread    *m_listeningThread;
	//TODO: base::BSONPtr m_operatorGraph;

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
	virtual void loadOperatorGraph( /*TODO:BSONPtr operatorGraph*/ )
	{
		//TODO:m_operatorGraph = operatorGraph;
		if( m_isConnected )
		{
			// send command to client
			std::cout << "send loadOperatorGraph" << std::endl;
			std::string test = "huhu";
			m_client.Send(test.c_str(), test.size());

			// todo:
			//BSONPtr command = BSON::create();
			//command["command"] = "loadOperatorGraph";
			//command["operatorGraph"] = m_operatorGraph;
			//unsigned char *commandPacket = BSON::pack( command );
			//m_client.Send(commandPacket, BSON::getPacketSize(commandPacket));

			// receive response
			//char buffer[1000];
			//std::cout << "sending test message..." << std::endl;
			//int numBytesReceived = client.Receive( buffer, 10000000 );
			//std::cout << numBytesReceived << std::endl;
		}
	}




	static void server( void *args )
	{
		RemoteApplication *_this = (RemoteApplication *)args;
		_this->m_server.Bind( IpEndpointName(12345) );
		_this->m_server.Listen();

		while( true )
		{
			std::cout << "listening for client..." << std::endl;
			_this->m_server.Accept( _this->m_client );
			if(_this->m_client.isValid())
			{
				std::cout << "connected to client..." << std::endl;
				_this->m_isConnected = true;

				_this->loadOperatorGraph();
			}else
				_this->m_isConnected = false;
		}
	}
};






int main(int argc, char ** argv)
{
	RemoteApplication remoteApp;

	// TODO:initialize application - this may be done later with a standalone driver
	//
	// base::BSONPtr opgraph = BSON::create();
	// base::BSONPtr operator = BSON::create();
	// base::BSONPtr clearOpData = BSON::create();
	// operator["type"] = "ClearOp";
	// clearOpData["clearColor"] = math::Vec3f(1.0f, 0.0f, 0.0f);
	// operator["data"] = clearOpData;
	// base::BSONPtr operators = BSON::create();
	// operators.append( operator )
	// opgraph["ops"] = operators;
	// opgraph["connections"] = operators;
	// opgraph["roots"]
	// remoteApp.loadOperatorGraph( opgraph );

	//Q_INIT_RESOURCE(application);
	QApplication app(argc, argv);
	app.setOrganizationName("composer");
	app.setApplicationName("composer");

	QMainWindow mainWin;
	mainWin.resize(800, 600);
	mainWin.show();


	return app.exec();
}
