#pragma once
#pragma push_macro("SendMessage")
#undef SendMessage

namespace app::fnd
{
	class CActor;

	// this class doesn't exist in the PC version at all and i don't know what to do with this
	class CActorTraverser : public ReferencedObject
	{
	public:
		virtual size_t Callback(CActor& actor) = 0;
	};

	// CActor : boost::noncopyable
	class CActor : public csl::ut::NonCopyable
	{
		friend MessageManager;
		
	protected:
		uint m_ActorID{};
		MessageManager* m_pMessageManager{};
		CActor* m_pParent{};
		csl::ut::Bitset<uint16> m_updateFlags {};
		char m_Flags{ 1 };
		bool m_Enabled{ true };
		unsigned int m_AllowedMessageFlags{ static_cast<unsigned>(-1) };
		bool isDeactivated{};

		void MessageSetup(uint to, fnd::Message& msg)
		{
			msg.m_Receiver = to;
			msg.m_Sender = m_ActorID;
		}
		
	public:
		void RemoveFromAllParents()
		{
			if (m_pParent)
				m_pParent->ActorProc(2, this);
		}
		
		virtual ~CActor()
		{
			RemoveFromAllParents();
			csl::fnd::Singleton<MessageManager>::GetInstance()->Remove(this);
		}
		
		virtual size_t ForEach(CActorTraverser& traverser) = 0;

	protected:
		virtual bool PreProcessMessage(Message& msg)
		{
			return false;
		}

		virtual bool ProcessMessage(Message& msg)
		{
			return PreProcessMessage(msg);
		}

		virtual void Update(const SUpdateInfo& info)
		{

		}

		virtual bool ActorProc(int id, void* data) = 0;

	public:
		size_t GetID() const
		{
			return m_ActorID;
		}
		
		void SetUpdateFlag(uint16 flag, bool value)
		{
			m_updateFlags.set(flag, value);
		}

		bool GetUpdateFlag(uint16 flag)
		{
			return m_updateFlags.test(flag);
		}
		
		bool SendMessageImm(uint to, fnd::Message& msg)
		{
			if (msg.mask & m_AllowedMessageFlags)
			{
				MessageSetup(to, msg);
				CActor* actor = m_pMessageManager->GetActor(to);
				
				if (!actor)
					return false;

				return actor->ActorProc(0, &msg);
			}
			return false;
		}

		bool BroadcastMessageImm(uint group, fnd::Message& msg)
		{
			if (msg.mask & m_AllowedMessageFlags)
			{
				MessageSetup(group, msg);

				CActor* actor = m_pMessageManager->GetActor(group);

				if (!actor)
					return false;

				return actor->ActorProc(1, &msg);
			}
			
			return false;
		}
		
		bool SendMessage(fnd::Message& msg)
		{
			if (m_AllowedMessageFlags & msg.mask)
			{
				MessageSetup(m_ActorID, msg);
				return ActorProc(0, &msg);
			}

			return false;
		}

		void SendMessage(uint to, fnd::Message& msg)
		{
			if (m_AllowedMessageFlags & msg.mask)
			{
				MessageSetup(to, msg);
				m_pMessageManager->AddMessage(msg);
			}
		}

		void BroadcastMessage(uint group, fnd::Message& msg)
		{
			MessageSetup(group, msg);
			msg.m_Broadcasted = true;
			m_pMessageManager->AddMessage(msg);
		}
	};
}

#pragma pop_macro("SendMessage")