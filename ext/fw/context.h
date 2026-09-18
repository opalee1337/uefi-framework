#pragma once

#include "fw.h"

namespace fw {

	class c_context {
	public:
		void init(efi::handle image_handle, efi::system_table* system_table) {
			m_image_handle = image_handle;
			m_system_table = system_table;
		}

		[[nodiscard]] efi::handle image_handle() const { return m_image_handle; }

		[[nodiscard]] efi::system_table* system_table() const { return m_system_table; }
		[[nodiscard]] efi::boot_services* boot_services() const { return m_system_table->boot_services; }
		[[nodiscard]] efi::runtime_services* runtime_services() const { return m_system_table->runtime_services; }

		[[nodiscard]] efi::simple_text_output* con_out() const { return m_system_table->con_out; }
		[[nodiscard]] efi::simple_text_input* con_in() const { return m_system_table->con_in; }

	private:
		efi::handle m_image_handle{ nullptr };
		efi::system_table* m_system_table{ nullptr };
	};

}

inline fw::c_context g_ctx;