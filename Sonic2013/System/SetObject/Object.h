#pragma once

namespace app::SetEd
{
	// Maybe this is somewhat big
	class CResObjectData 
	{
		uint32_t m_ObjectID;
		void* m_Unk1;
		Vector3 m_Position;
		void* m_Unk2;
		uint32_t m_ParentID;
		void* m_Unk3;
	};

	class CResObject : fnd::CResCommon<CResObjectData>
	{
		
	};
}