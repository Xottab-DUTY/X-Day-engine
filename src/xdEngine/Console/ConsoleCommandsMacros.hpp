#pragma once

#define CMDA(ccname) { Console.AddCommand(&ccname); }

#define CMD0(cls)                       { static cls cls##ccname();                 Console.AddCommand(&cls##ccname); }
#define CMD1(cls, p1)                   { static cls cls##ccname(p1);               Console.AddCommand(&cls##ccname); }
#define CMD2(cls, p1, p2)               { static cls cls##ccname(p1,p2);            Console.AddCommand(&cls##ccname); }
#define CMD3(cls, p1, p2, p3)           { static cls cls##ccname(p1,p2,p3);         Console.AddCommand(&cls##ccname); }
#define CMD4(cls, p1, p2, p3, p4)       { static cls cls##ccname(p1,p2,p3,p4);      Console.AddCommand(&cls##ccname); }
#define CMD5(cls, p1, p2, p3, p4, p5)   { static cls cls##ccname(p1,p2,p3,p4,p5);   Console.AddCommand(&cls##ccname); }

#define CMDN0(cls, ccname)                      { static cls ccname();                 Console.AddCommand(&ccname); }
#define CMDN1(cls, ccname, p1)                  { static cls ccname(p1);               Console.AddCommand(&ccname); }
#define CMDN2(cls, ccname, p1, p2)              { static cls ccname(p1,p2);            Console.AddCommand(&ccname); }
#define CMDN3(cls, ccname, p1, p2, p3)          { static cls ccname(p1,p2,p3);         Console.AddCommand(&ccname); }
#define CMDN4(cls, ccname, p1, p2, p3, p4)      { static cls ccname(p1,p2,p3,p4);      Console.AddCommand(&ccname); }
#define CMDN5(cls, ccname, p1, p2, p3, p4, p5)  { static cls ccname(p1,p2,p3,p4,p5);   Console.AddCommand(&ccname); }
