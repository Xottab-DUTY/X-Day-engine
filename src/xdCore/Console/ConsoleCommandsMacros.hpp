#pragma once

#define CMDA(cmd) { Commands::Register(&cmd); }

#define CMD0(type)                     { static Command<type> cmd;                     Commands::Register(&cmd); }
#define CMD1(type, p1)                 { static Command<type> cmd(p1);                 Commands::Register(&cmd); }
#define CMD2(type, p1, p2)             { static Command<type> cmd(p1, p2);             Commands::Register(&cmd); }
#define CMD3(type, p1, p2, p3)         { static Command<type> cmd(p1, p2, p3);         Commands::Register(&cmd); }
#define CMD4(type, p1, p2, p3, p4)     { static Command<type> cmd(p1, p2, p3, p4);     Commands::Register(&cmd); }
#define CMD5(type, p1, p2, p3, p4, p5) { static Command<type> cmd(p1, p2, p3, p4, p5); Commands::Register(&cmd); }
