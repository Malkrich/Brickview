#pragma once

namespace Brickview
{

	class GraphicsContext
	{
	public:
		static Scope<GraphicsContext> create(void* window);

		virtual ~GraphicsContext() = default;

		virtual void init() = 0;
		virtual void swapBuffers() = 0;
	};

}