
NEVENTS ?= 5

help: Makefile ## Print this help text
	@perl -nle 'printf("  %-20s %s\n",$$1,$$2) if /^(\S+):.*##\s*(.*)/' $^

draw: ## Draw digits and hits
	root draw.C

sim: data/trddigits.root ## Run Geant simulation of the TRD

.PRECIOUS: %/o2sim_HitsTRD.root %/trddigits.root
%/o2sim_HitsTRD.root:
	mkdir -p $(dir $@)
	cd $(dir $@); o2-sim -n $(NEVENTS)

%/trddigits.root: %/o2sim_HitsTRD.root
	cd $(dir $@); o2-sim-digitizer-workflow -b --onlyDet=TRD

clean: ## Remove compiled files
	rm -rf *_C.d *_C.so *_C_ACLiC_dict_rdict.pcm AutoDict* core_dump_*

reallyclean: clean ## Remove compiled files and simulation outputs
	rm -rf padrow_*.pdf data/
