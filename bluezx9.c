/***************************************************************************
 * Copyright (C) 2022, Lanka Hsu, <lankahsu@gmail.com>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
#include "bluezx9.h"

static BlueZX9_t *bluezx9_req_last = NULL;

int bluezx9_ctrl_pairable_set(char *str)
{
	int ret = 0;

	if ((bluezx9_req_last) && (str) )
	{
		char val[LEN_OF_BUF128] = "";
		SAFE_SPRINTF(val, "%s", str);
		bluezx9_exec_helper(shell_exec_helper, bluezx9_req_last, "pairable", val, NULL);

		{
			ThreadX_t *tidx_req = &bluezx9_req_last->tidx;
			threadx_timewait_simple(tidx_req, 2000);
		}

		{
			CTRL_t *ctrl = &bluezx9_req_last->ctrl;
			if ( ( SAFE_STRSTR(str, "on") ) && (ctrl->pairable == 1) )
			{
				DBG_DB_LN("(ctrl->pairable: %d, str: %s)", ctrl->pairable, str);
				ret = 0;
			}
			else if ( ( SAFE_STRSTR(str, "off") ) && (ctrl->pairable == 0) )
			{
				DBG_DB_LN("(ctrl->pairable: %d, str: %s)", ctrl->pairable, str);
				ret = 0;
			}
			else
			{
				ret = -1;
			}
		}
	}
	else
	{
		ret = -1;
	}

	return ret;
}

void bluezx9_ctrl_pairable_cb(char *str)
{
	DBG_DB_LN("(str: %s)", str);

	if (bluezx9_req_last)
	{
		CTRL_t *ctrl = &bluezx9_req_last->ctrl;
		if ( SAFE_STRSTR(str, "on") )
		{
			ctrl->pairable = 1;
		}
		else
		{
			ctrl->pairable = 0;
		}

		{
			ThreadX_t *tidx_req = &bluezx9_req_last->tidx;
			threadx_wakeup_simple(tidx_req);
		}
	}
}

void bluezx9_ctrl_name_cb(char *name)
{
	DBG_DB_LN("(name: %s)", name);

	if (bluezx9_req_last)
	{
		CTRL_t *ctrl = &bluezx9_req_last->ctrl;
		SAFE_SPRINTF_EX(ctrl->name, "%s", name);
	}
}

void bluezx9_ctrl_address_and_name_cb(char *address, char *name)
{
	DBG_DB_LN("(address: %s, name: %s)", address, name);

	if (bluezx9_req_last)
	{
		CTRL_t *ctrl = &bluezx9_req_last->ctrl;
		SAFE_SPRINTF_EX(ctrl->address, "%s", address);
		SAFE_SPRINTF_EX(ctrl->name, "%s", name);
	}
}

void bluezx9_gatt_chrc_write_value_cb(struct chrc *chrc, char *device, char *link)
{
	char *chrc_uuid = chrc->uuid;
	struct service *service = chrc->service;
	char *service_uuid = service->uuid;

	DBG_DB_LN("(service_uuid: %s, chrc_uuid: %s, device: %s, link: %s)", service_uuid, chrc_uuid, device, link);

	if ( (bluezx9_req_last) && (bluezx9_req_last->gatt_chrc_write_value_cb) )
	{
		bluezx9_req_last->gatt_chrc_write_value_cb(bluezx9_req_last, chrc, device, link);
	}
}

void bluezx9_gatt_application_registered_cb(void)
{
	if ( (bluezx9_req_last) && (bluezx9_req_last->gatt_application_registered_cb) )
	{
		bluezx9_req_last->gatt_application_registered_cb(bluezx9_req_last);
	}
}

void bluezx9_gatt_register_service_cb(struct service *service)
{
	if ( (bluezx9_req_last) && (bluezx9_req_last->gatt_application_registered_cb) )
	{
		bluezx9_req_last->gatt_register_service_cb(bluezx9_req_last, service);
	}
}

void bluezx9_gatt_register_chrc_cb(struct chrc *chrc)
{
	if ( (bluezx9_req_last) && (bluezx9_req_last->gatt_application_registered_cb) )
	{
		bluezx9_req_last->gatt_register_chrc_cb(bluezx9_req_last, chrc);
	}
}

void bluezx9_advertising_set(BlueZX9_t *bluezx9_req, int isadvertising)
{
	if (bluezx9_req)
	{
		bluezx9_req->isadvertising = isadvertising;
		DBG_WN_LN("(bluezx9_req->isadvertising: %d)", bluezx9_req->isadvertising);
	}
}

int bluezx9_advertising(BlueZX9_t *bluezx9_req)
{
	return bluezx9_req->isadvertising;
}

void bluezx9_advertising_registered_cb(void)
{
	if ( (bluezx9_req_last) && (bluezx9_req_last->advertising_registered_cb) )
	{
		bluezx9_advertising_set(bluezx9_req_last, 1);
		bluezx9_req_last->advertising_registered_cb(bluezx9_req_last);
	}
}

void bluezx9_advertising_unregistered_cb(void)
{
	if ( (bluezx9_req_last) && (bluezx9_req_last->advertising_unregistered_cb) )
	{
		bluezx9_advertising_set(bluezx9_req_last, 0);
		bluezx9_req_last->advertising_unregistered_cb(bluezx9_req_last);
	}
}

void bluezx9_advertising_released_cb(void)
{
	if ( (bluezx9_req_last) && (bluezx9_req_last->advertising_released_cb) )
	{
		bluezx9_advertising_set(bluezx9_req_last, 0);
		bluezx9_req_last->advertising_released_cb(bluezx9_req_last);
	}
}

void bluezx9_agnet_set(BlueZX9_t *bluezx9_req, int isagent)
{
	if (bluezx9_req_last)
	{
		bluezx9_req_last->isagent = isagent;
		DBG_WN_LN("(bluezx9_req_last->isagent: %d)", bluezx9_req_last->isagent);
	}
}

int bluezx9_agent(BlueZX9_t *bluezx9_req)
{
	if (bluezx9_req_last)
	{
		return bluezx9_req_last->isagent;
	}

	return bluezx9_req->isagent;
}

void bluezx9_agent_register_reply_cb(DBusMessage *message, void *user_data)
{
	if (bluezx9_req_last)
	{
		bluezx9_agnet_set(bluezx9_req_last, 1);
	}
}

void bluezx9_agent_release_cb(void)
{
	if (bluezx9_req_last)
	{
		bluezx9_agnet_set(bluezx9_req_last, 0);
	}
}

void bluezx9_client_ready_set(BlueZX9_t *bluezx9_req, READY_ID ready)
{
	if (bluezx9_req)
	{
		bluezx9_req->isready = ready;
	}
}

READY_ID bluezx9_client_ready(BlueZX9_t *bluezx9_req)
{
	return bluezx9_req->isready;
}

void bluezx9_client_ready_cb(GDBusClient *client, void *user_data)
{
	BlueZX9_t *bluezx9_req = (BlueZX9_t *)user_data;
	if (bluezx9_req)
	{
		bluezx9_client_ready_set(bluezx9_req, READY_ID_OK);

		{
			CTRL_t *ctrl = &bluezx9_req->ctrl;
			char val[LEN_OF_BUF128] = "";
			if ( 0 == bluezx9_ctrl_property_get("Pairable", val) )
			{
				if ( SAFE_STRSTR(val, "yes") )
				{
					ctrl->pairable = 1;
				}
				else
				{
					ctrl->pairable = 0;
				}
			}
		}

		if (bluezx9_req->client_ready_cb)
		{
			bluezx9_req->client_ready_cb(client, user_data);
		}
	}
}

void bluezx9_proxy_added_cb(GDBusProxy *proxy, void *user_data)
{
	BlueZX9_t *bluezx9_req = (BlueZX9_t *)user_data;
	if (bluezx9_req)
	{
		if (bluezx9_req->proxy_added_cb)
		{
			bluezx9_req->proxy_added_cb(proxy, user_data);
		}
	}
}

void bluezx9_proxy_removed_cb(GDBusProxy *proxy, void *user_data)
{
	BlueZX9_t *bluezx9_req = (BlueZX9_t *)user_data;
	if (bluezx9_req)
	{
		if (bluezx9_req->proxy_removed_cb)
		{
			bluezx9_req->proxy_removed_cb(proxy, user_data);
		}
		
	}
}

void bluezx9_property_changed_cb(GDBusProxy *proxy, const char *name, DBusMessageIter *iter, void *user_data)
{
	BlueZX9_t *bluezx9_req = (BlueZX9_t *)user_data;
	if (bluezx9_req)
	{
		if (bluezx9_req->property_changed_cb)
		{
			bluezx9_req->property_changed_cb(proxy, name, iter, user_data);
		}
		
	}
}

void bluezx9_connect_handler_cb(DBusConnection *connection, void *user_data)
{
	BlueZX9_t *bluezx9_req = (BlueZX9_t *)user_data;
	if (bluezx9_req)
	{
		if (bluezx9_req->connect_handler_cb)
		{
			bluezx9_req->connect_handler_cb(connection, user_data);
		}
		
	}
}

void bluezx9_disconnect_handler_cb(DBusConnection *connection, void *user_data)
{
	BlueZX9_t *bluezx9_req = (BlueZX9_t *)user_data;
	if (bluezx9_req)
	{
		if (bluezx9_req->disconnect_handler_cb)
		{
			bluezx9_req->disconnect_handler_cb(connection, user_data);
		}
		
	}
}

void bluezx9_message_handler_cb(DBusConnection *connection, DBusMessage *message, void *user_data)
{
	BlueZX9_t *bluezx9_req = (BlueZX9_t *)user_data;
	if (bluezx9_req)
	{
		if (bluezx9_req->message_handler_cb)
		{
			bluezx9_req->message_handler_cb(connection, message, user_data);
		}
		
	}
}

static void *bluezx9_thread_handler(void *user)
{
	BlueZX9_t *bluezx9_req = (BlueZX9_t*)user;

	if (bluezx9_req)
	{
		ThreadX_t *tidx_req = &bluezx9_req->tidx;
		threadx_detach(tidx_req);

		int count = 0;
		DBG_IF_LN("(count: %d)", count++);

		if (1)
		{
			//bluezx9_exec_helper(bluetoothctl_start, "blueway", "--repeater", NULL);
			DBG_WN_LN("bluezx9 listen ...");
			bluezx9_exec_helper(bluetoothctl_start, bluezx9_req, "./blueway", "--repeater", NULL);
		}
		else
		{
			while (threadx_isquit(tidx_req)==0)
			{
				DBG_IF_LN("(count: %d)", count++);
				sleep(1);
			}
		}

		threadx_leave(tidx_req);
	}
	DBG_IF_LN(DBG_TXT_BYE_BYE);

	return NULL;
}

int bluezx9_exec_helper(exec_fn cb, void *user_data, char *action, ...)
{
	int ret = 0;
	int argc = 0;
	char *argv[MAX_OF_EXEC_ARGC];

	if (action)
	{
		//argv[argc++] = action;
		SAFE_ASPRINTF(argv[argc++], "%s", action);
		va_list vargs;
		va_start(vargs, action);
		char *s = va_arg(vargs, char*);
		while (s)
		{
			SAFE_ASPRINTF(argv[argc++], "%s", s);
			s = va_arg(vargs, char*);
		}
		va_end(vargs);

		ret = cb(argc,  argv, user_data);

		int i = 0;
		for (i=0; i<argc; i++)
		{
			SAFE_FREE(argv[i]);
		}
	}

	return ret;
}

int bluezx9_thread_init(BlueZX9_t *bluezx9_req, char *save_path)
{
	if ((bluezx9_req == NULL) || (save_path==NULL))
	{
		return -1;
	}

	bluezx9_req_last = bluezx9_req;

	SAFE_SPRINTF(bluezx9_req->save_path, "%s", save_path);

	CLIST_STRUCT_INIT(bluezx9_req, chrc_list);

	{
		ThreadX_t *tidx_req = &bluezx9_req->tidx;
		tidx_req->thread_cb = bluezx9_thread_handler;
		tidx_req->data = bluezx9_req;
		threadx_init(tidx_req, bluezx9_req->name);
	}

	return 0;
}

void bluezx9_thread_stop(BlueZX9_t *bluezx9_req)
{
	bt_shell_quit(EXIT_SUCCESS);

	if (bluezx9_req)
	{
		ThreadX_t *tidx_req = &bluezx9_req->tidx;
		threadx_stop(tidx_req);
	}
}

void bluezx9_thread_close(BlueZX9_t *bluezx9_req)
{
	if ((bluezx9_req) && (bluezx9_req->isfree == 0))
	{
		bluezx9_req->isfree ++;
	
		ThreadX_t *tidx_req = &bluezx9_req->tidx;
		threadx_close(tidx_req);

		clist_free(bluezx9_req->chrc_list);

		bluezx9_req_last = NULL;
	}
}

