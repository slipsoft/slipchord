#define INIT          0 // Message to init the peertable
#define DONE          1 // Message to propagate the peertable
#define VALUE         2 // Value sent by the simulator
#define KEYEXP        3 // Key_exposant sent by the simulator
#define REVERSE       4 // Message to signal a reverse finger
#define END           5 // Message to Quit the application
#define PRINT         6 // Message to trigger a print
#define INIT_END      7 // Message to notify the simulator that init is ended

#define TAG_RECHERCHE_INIT   100 // recherche d'une clef : demande depuis le processus init
#define TAG_RECHERCHE_PAIR   101 // recherche d'une clef : demande depuis un pair
#define TAG_RECHERCHE_TROUVE 102 // recherche d'une clef : le responsable envoie que c'est lui