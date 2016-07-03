/*!
	@file
	@author		Albert Semenov
	@date		04/2009
*/

#ifndef __MYGUI_OGRE_VERTEX_BUFFER_H__
#define __MYGUI_OGRE_VERTEX_BUFFER_H__

#include "MyGUI_Prerequest.h"
#include "MyGUI_IVertexBuffer.h"

#include <OGRE\OgreHardwareBufferManager.h>
#include <OGRE\OgreHardwareVertexBuffer.h>
#include <OGRE\OgrePrerequisites.h>
#include <OGRE\OgreRenderOperation.h>
#include <OGRE\OgreRenderSystem.h>
#include <OGRE\OgreTextureManager.h>
#include <OGRE\OgreTextureUnitState.h>

#include "MyGUI_LastHeader.h"

namespace MyGUI
{

	class OgreVertexBuffer :
		public IVertexBuffer
	{
	public:
		OgreVertexBuffer();
		virtual ~OgreVertexBuffer();

		virtual void setVertexCount(size_t _count);
		virtual size_t getVertexCount();

		virtual Vertex* lock();
		virtual void unlock();

		Ogre::RenderOperation* getRenderOperation()
		{
			return &mRenderOperation;
		}

	private:
		void createVertexBuffer();
		void destroyVertexBuffer();
		void resizeVertexBuffer();

	private:
		size_t mVertexCount;
		size_t mNeedVertexCount;

		Ogre::RenderOperation mRenderOperation;
		Ogre::HardwareVertexBufferSharedPtr mVertexBuffer;
	};

} // namespace MyGUI

#endif // __MYGUI_OGRE_VERTEX_BUFFER_H__
