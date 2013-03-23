#
# Makefile
#

LIBS = -lpthread

default: fire

OFILES = main.o \
                    50move.o \
                    all_node.o \
                    arrays.o \
                    benchmark.o \
                    control.o \
                    cut_node.o \
                    endgame.o \
                    evaluation.o \
                    exclude_node.o \
                    hash_init.o \
                    hash_use.o \
                    init.o \
                    input.o \
                    low_depth.o \
                    magic_mult.o \
                    make_move.o \
                    material_value.o \
                    mem_handler.o \
                    mobility.o \
                    move_gen.o \
                    next_move.o \
                    ok_move.o \
                    pawn_eval.o \
                    pv_node.o \
                    qsearch.o \
                    qsearch_pv.o \
                    robbo_cache.o \
                    robbo_comp.o \
                    robbo_count.o \
                    robbo_decomp.o \
                    robbo_glue.o \
                    robbo_init.o \
                    robbo_init_IO.o \
                    robbo_suffix.o \
                    robbo_triple.o \
                    robbo_triple_disk.o \
                    robbo_triple_init.o \
                    robbo_triple_value.o \
                    robbo_utility.o \
                    root_analysis.o \
                    root_multipv.o \
                    root_node.o \
                    search.o \
                    SEE.o \
                    set_position.o \
                    signals.o \
                    slab_memory.o \
                    SMP.o \
                    SMP_init.o \
                    SMP_search.o \
                    static.o \
                    top_analysis.o \
                    top_node.o \
                    unmake_move.o \
                    utility.o \
                 
     
$(OFILES): 

fire:  $(OFILES) 
	gcc -o fire $(OFILES)  $(LIBS) 

clean:
	rm -f *.o  fire

.c.o:   main.c
	gcc -c -g $*.c  -Wall -w -pipe  -s -std=c99 -DNDEBUG -Ofast -static  \
                       -flto -fwhole-program -finline-functions -fprefetch-loop-arrays  \
                      -Wfatal-errors -DHasPreFetch -DRobboBases  -mtune=native \
                    

# for popcount (AMD)          =   -march=amdfam10 -mtune=amdfam10 -mpopcnt -DHasPopCNT
# for popcount (INTEL)        =   -msse4.2 -march=corei7 -mtune=corei7 -mpopcnt -DHasPopCNT

# for RobboBase support  =  -DRobboBases

# for Windows builds (otherwise Linux)  =  -DWINDOWS

# for SSE cpus (and above)  =  -DHasPreFetch







