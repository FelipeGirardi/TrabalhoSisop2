###
# Makefile to build app_client and app_server applications.
#
# Authors:
#   Felipe Girardi 
#   Laura Corssac
#   Marlize Ramos
#   Renan Kummer
###

all: common app_client app_server

common:
	@$(MAKE) -C common

app_client:
	@$(MAKE) -C app_client

app_server:
	@$(MAKE) -C app_server

.PHONY: common app_client app_server

clean:
	@$(MAKE) clean -C common
	@$(MAKE) clean -C app_client
	@$(MAKE) clean -C app_server
