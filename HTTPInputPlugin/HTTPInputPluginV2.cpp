// HTTPInputPluginV2.cpp
// Una interfaz menos intrusiva con el bucle principal de Vigasoco
// Necesita usarse de forma combinada con el plugin PollEvent 
// de SDLInputKeyboardPlugin.so
//
/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include "HTTPInputPluginV2.h"

#include "crow_all.h"


/////////////////////////////////////////////////////////////////////////////
// initialization and cleanup
/////////////////////////////////////////////////////////////////////////////

HTTPInputPluginV2::HTTPInputPluginV2()
{
	initRemapTable();
}

HTTPInputPluginV2::~HTTPInputPluginV2()
{
}


void HTTPInputPluginV2::simulateKeys(int repeat, int interval, std::vector<char *> keys )
{
	std::thread keyThread([repeat,interval,keys]() {
		fprintf(stderr,"dentro repeat %d interval %d key0 %s size %d\n",
			repeat,interval,keys[0],(int)keys.size()); 
		int r=repeat;
		//while (repeat--) {
		while (r--) {
			for(int i=0;i<keys.size();i++) 
			{	
				SDL_Event sdlevent = {};
				sdlevent.type = SDL_KEYDOWN;
				sdlevent.key.state=SDL_PRESSED;
				fprintf(stderr,"i %d keys[i] %s atoi %d\n",
					i,keys[i],std::atoi(keys[i]));
				sdlevent.key.keysym.sym = SDLKey(std::atoi(keys[i]));
				SDL_PushEvent(&sdlevent);
			} 
			std::this_thread::sleep_for(std::chrono::milliseconds(interval));
		} 
	});
 	// no podemos hacer detach porque desapareceria del contexto la variable keys que usa el hilo que envia los eventos
	//keyThread.detach();
	keyThread.join();  // para esto no montamos un hilo, pero mientras arreglamos lo del contexto
}

bool HTTPInputPluginV2::init()
{
	std::thread webThread([this]() {
		crow::SimpleApp app;

		CROW_ROUTE(app, "/reset")([this](){
//TODO : poner constantes para  10 y 25 que se puedan usar varias veces por el codigo
			std::vector<char *> keys;
			char rEset[]="101"; // TODO, convertir a cadena SDLK_e
			keys.push_back(rEset);
//			keys.push_back(std::to_string(SDLK_e).c_str());
// y si en vez de pasar un array simplemente pasamos una cadena que recorremos
// y cada char un caracter ¿puede ser problema con alguna tecla ?
// o seguir usando SDLKeys ...???
			simulateKeys(10,25,keys);
                	return crow::response(200);
        	});		

		// ejemplo enviar QR . SDLK_q=113 y SDLK_r=114
		// /simulate?repeat=10&interval=25&keys[]=113&keys[]=114	
		CROW_ROUTE(app, "/simulate")([this](crow::request req){
			int repeat=10;   // cuantas veces repetir el envio del evento
			int interval=25; // ms entre repeticion
			if(req.url_params.get("repeat") != nullptr) {
		            repeat = boost::lexical_cast<int>(req.url_params.get("repeat"));
        		};
			if(req.url_params.get("interval") != nullptr) {
		            interval = boost::lexical_cast<int>(req.url_params.get("interval"));
        		};
			auto keys = req.url_params.get_list("keys");
			//std::vector<char *> keys = req.url_params.get_list("keys");
			if (keys.size()<1) crow::response(400);
fprintf(stderr,"fuera repeat %d interval %d key0 %s size %d\n",repeat,interval,keys[0],(int)keys.size()); 

			simulateKeys(repeat,interval,keys);

                	return crow::response(200);
		});

		app.port(8182).run();
	});
	webThread.detach();
	return true;
}

void HTTPInputPluginV2::end()
{
}

void HTTPInputPluginV2::acquire()
{
}

void HTTPInputPluginV2::unAcquire()
{
}

/////////////////////////////////////////////////////////////////////////////
// input processing
/////////////////////////////////////////////////////////////////////////////

bool HTTPInputPluginV2::process(int *inputs)
{
	// El servidor web no modifica ninguna variable de esta clase para indicr
	// que se debe devolver false en process para salir del juego
	// En su lugar inyecta el evento SDL_QUIT para que se procese adecuadamente
	// con el plugin PollEvent
	return true; 
}

/////////////////////////////////////////////////////////////////////////////
// helper methods
/////////////////////////////////////////////////////////////////////////////

void HTTPInputPluginV2::initRemapTable()
{
}

/////////////////////////////////////////////////////////////////////////////
// Custom plugin properties
/////////////////////////////////////////////////////////////////////////////

const std::string HTTPInputPluginV2::g_properties[] = {
};

const int HTTPInputPluginV2::g_paramTypes[] = {
};

const int * HTTPInputPluginV2::getPropertiesType() const
{
	return HTTPInputPluginV2::g_paramTypes;
}

const std::string * HTTPInputPluginV2::getProperties(int *num) const 
{
	*num = sizeof(g_paramTypes)/sizeof(g_paramTypes[0]);
	return HTTPInputPluginV2::g_properties;
}

void HTTPInputPluginV2::setProperty(std::string prop, int data)
{
}

void HTTPInputPluginV2::setProperty(std::string prop, int index, int data)
{
}

int HTTPInputPluginV2::getProperty(std::string prop) const
{
	return -1;
};

int HTTPInputPluginV2::getProperty(std::string prop, int index) const
{
	return -1; 
};