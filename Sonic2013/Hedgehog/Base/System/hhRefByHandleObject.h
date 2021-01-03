#pragma once

namespace app::fnd
{
	class RefByHandleObject;
	
	struct HandleTableEntry
	{
		size_t handle{};
		RefByHandleObject* object{};
	};
	
	class RefByHandleObject : public ReferencedObject
	{
	private:
		size_t handle{};
		HandleTableEntry* entry{};

	public:
		~RefByHandleObject() override
		{
			csl::ut::Singleton<GameObjectSystem>::GetInstance()->handleManager->RemoveObject(*this);
		}
		
		RefByHandleObject()
		{
			csl::ut::Singleton<GameObjectSystem>::GetInstance()->handleManager->AddObject(*this);
		}

	protected:
		RefByHandleObject(bool skip)
		{
			
		}
	};
}