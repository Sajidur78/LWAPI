#pragma once

namespace app::fnd
{
	class CBranchActor : public CActor
	{
	protected:
		csl::ut::MoveArray<CActor*> m_ChildActors { 1024, game::GlobalAllocator::GetSingletonAllocator() };
		csl::ut::MoveArray<CActor*> m_PhasedActors[3];

	public:
		CBranchActor()
		{
			for (size_t i = 0; i < 3; i++)
			{
				m_PhasedActors[i] = csl::ut::MoveArray<CActor*>(1024, game::GlobalAllocator::GetSingletonAllocator());
			}
		}

		size_t ForEach(CActorTraverser& traverser) override
		{
			auto count = traverser.Callback(*this);
			if (count)
				return count == 1;

			for (auto& pActor : m_ChildActors)
			{
				pActor->ForEach(traverser);
			}
			
			return 1;
		}
	};
}