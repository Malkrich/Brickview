#pragma once



namespace Brickview
{

	class Log
	{
	public:
		static void Init();



	private:
		static Log* s_instance;
	};

}

#ifdef BV_DEBUG
	#define BV_LOG_INFO(text) 

	#define BV_ASSERT(condition, message) if(!condition) { BV_LOG_INFO(message); }
#else
	#define BV_LOG_INFO(text)

	#define BV_ASSERT(condition, message)
#endif