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
#include <streambuf>


#include <ui/GLViewer.h>
#include <gltools/gl.h>
#include <gltools/misc.h>
#include <util/StringManip.h>
#include <util/Path.h>
#include <util/BSON.h>
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


#include <sstream>

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
	bool                          m_isConnected;
	TcpSocket                          m_server;
	TcpSocket                          m_client;
	tthread::thread          *m_listeningThread;
	base::bson::BSONPtr         m_operatorGraph;

	RemoteApplication() : m_isConnected(false), m_listeningThread(0)
	{
	}

	~RemoteApplication()
	{
		if(m_listeningThread)
		{
			m_listeningThread->join();
			delete m_listeningThread;
			m_listeningThread = 0;
		}
	}

	// play
	// stop
	// setTime();
	// load( std::string operatorPath, BSONPtr data )
	virtual void loadOperatorGraph( base::bson::BSONPtr operatorGraph )
	{
		m_operatorGraph = operatorGraph;
		if( m_isConnected )
		{
			// send command to client
			std::cout << "send loadOperatorGraph" << std::endl;
			//std::string test = "huhu";
			//m_client.Send(test.c_str(), test.size());

			// todo:
			base::bson::Helper command = base::bson::create();
			command["command"] = "loadOperatorGraph";
			command["operatorGraph"] = m_operatorGraph;
			base::bson::PacketPtr packet = base::bson::pack( command );
			m_client.Send(packet->m_data, packet->m_size);

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

				_this->loadOperatorGraph(_this->m_operatorGraph);
			}else
				_this->m_isConnected = false;
		}
	}

	void startServer()
	{
		// start server thread
		m_listeningThread = new tthread::thread(server, this);
	}
};






int main(int argc, char ** argv)
{

	RemoteApplication remoteApp;

	// TODO:initialize application - this may be done later with a standalone driver
	//
	base::bson::Helper opgraph = base::bson::create();
	base::bson::Helper op = base::bson::create();
	base::bson::Helper clearOpData = base::bson::create();

	op["type"] = "ClearOp";
	clearOpData["clearColor"] = math::Vec3f(1.0f, 0.0f, 0.0f);
	op["data"] = clearOpData;

	base::bson::Helper operators = base::bson::create();
	operators += op;
	//operators["safsf"] = op;

	opgraph["ops"] = operators;
	//opgraph["connections"] = operators;
	//opgraph["roots"];
	remoteApp.loadOperatorGraph( opgraph );

	remoteApp.startServer();


	//test
	if(0)
	{
		// tested datatypes:
		// string keys, int keys
		// int, float, string, vec3f, bool, bson(nested)
		
		// todo: array, binary blobs

		int in_int = 13;
		std::string in_str = "testme";
		float in_float = 1.2f;
		math::Vec3f in_vec3f = math::Vec3f( 0.1f, 0.2f, 0.3f );
		bool in_bool = true;

		base::bson::Helper in = base::bson::create();
		in["int"] = in_int;
		in["str"] = in_str;
		in["float"] = in_float;
		in[0] = in_int;

		base::bson::Helper in_bson = base::bson::create();
		in_bson["int"] = in_int;
		in_bson["str"] = in_str;
		in_bson["float"] = in_float;
		in_bson["vec3f"] = in_vec3f;
		in_bson["bool"] = in_bool;
		in_bson += std::string("test");
		in["bson"] = in_bson;

		base::bson::PacketPtr packet = base::bson::pack( in );
		base::bson::Helper out = base::bson::unpack( packet );



		int out_int = 0;
		int out_int2 = 0;
		std::string out_str = "";
		float out_float = 0.0f;
		math::Vec3f out_vec3f(0.0f, 0.0f, 0.0f);
		bool out_bool = false;
		base::bson::Helper out_bson = out["bson"];


		out_int = out["int"];
		out_str = out["str"].asString();
		out_float = out["float"];
		out_int2 = out[0];


		std::cout << out_int << std::endl;
		std::cout << out_str << std::endl;
		std::cout << out_float << std::endl;

		out_int = out_bson["int"];
		out_str = out_bson["str"].asString();
		out_float = out_bson["float"];
		out_vec3f = out_bson["vec3f"];
		out_bool = out_bson["bool"];

		std::cout << "from out_bson:" << std::endl;
		std::cout << out_int << std::endl;
		std::cout << out_int2 << std::endl;
		std::cout << out_str << std::endl;
		std::cout << out_float << std::endl;
		std::cout << out_vec3f.x << " " << out_vec3f.y << " " << out_vec3f.z << std::endl;
		std::cout << (out_bool ? "true" : "false") << std::endl;
		std::cout << out_bson[0].asString() << std::endl;
	}

	//Q_INIT_RESOURCE(application);
	QApplication app(argc, argv);
	app.setOrganizationName("composer");
	app.setApplicationName("composer");

	QMainWindow mainWin;
	mainWin.resize(800, 600);
	mainWin.show();


	return app.exec();
}
