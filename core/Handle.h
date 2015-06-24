#pragma once

#include <WTypes.h>
#include <memory>

namespace mswin
{
	class ScopedHandle
	{
	private:
		struct HandleContext
		{
			HandleContext(HANDLE h)
			: m_h(h)
			{
			}

			~HandleContext()
			{
				if (m_h && m_h != INVALID_HANDLE_VALUE)
					::CloseHandle(m_h);
			}

			HANDLE m_h;
		};

	public:
		ScopedHandle()
		: m_pHandle(new HandleContext(nullptr))
		{
		}

		ScopedHandle(HANDLE h)
		: m_pHandle(new HandleContext(h))
		{
		}

		// Copy constructor
		ScopedHandle(const ScopedHandle& rhs)
		{
			m_pHandle = rhs.m_pHandle;
		}

		~ScopedHandle()
		{
			m_pHandle.reset();
		}

		HANDLE* Reset()
		{
			CloseHandle();
			return &m_pHandle->m_h;
		}

		operator HANDLE() const
		{
			return m_pHandle->m_h;
		}

		ScopedHandle& operator=(const HANDLE h)
		{
			// Only update the internal handle if it's different
			if (m_pHandle->m_h != h)
			{
				CloseHandle();
				m_pHandle->m_h = h;
			}

			return *this;
		}

		ScopedHandle& operator=(const ScopedHandle& sh)
		{
			// Only update the internal handle if it's different
			if (m_pHandle->m_h != sh.m_pHandle->m_h)
			{
				m_pHandle = sh.m_pHandle;
			}

			return *this;
		}

		void CloseHandle()
		{
			m_pHandle.reset(new HandleContext(nullptr));
		}

	private:
		std::shared_ptr<HandleContext> m_pHandle;
	};

} // namespace mswin