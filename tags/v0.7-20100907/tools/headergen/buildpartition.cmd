@if exist partition.mbn rm partition.mbn
@make -f partition.mak %1
@echo ------------------------------------------------------------
@echo partition.mbn is Generated.
@echo ------------------------------------------------------------


