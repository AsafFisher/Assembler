include "protocols.h"
Command createInstaceOfCommand(int pERA,int pdestar,int psrcar,int popcode,int pgrp){
	Command mycmd;
	mycmd.dumy = 101b;
	mycmd.ERA = pERA;
	mycmd.destar = pdestar;
	mycmd.srcar = psrcar;
	mycmd.opcode = popcode;
	mycmd.grp = pgrp
	return mycmd;
}
Command createInstaceOfCommand(int pERA,int pdestar,int psrcar,int popcode,int pgrp){
	Command mycmd;
	mycmd.dumy = 101b;
	return mycmd;
}
