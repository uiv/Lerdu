makesis   ..\..\sis\GSTRegistryGenerator.pkg   .\GSTRegistryGenerator.sis
signsis   .\GSTRegistryGenerator.sis   .\GSTRegistryGenerator.sisx RD.crt RD.key

makesis   ..\..\sis\GSTRegistryGenerator_update.pkg   .\GSTRegistryGenerator_update.sis
signsis   .\GSTRegistryGenerator_update.sis   .\GSTRegistryGenerator_update.sisx RD.crt RD.key

makesis   .\good_plugins_dlls.pkg   .\good_plugins_dlls.sis
signsis   .\good_plugins_dlls.sis   .\good_plugins_dlls.sisx RD.crt RD.key

makesis   .\good_plugins_armv5.pkg   .\good_plugins_armv5.sis
signsis   .\good_plugins_armv5.sis   .\good_plugins_armv5.sis RD.crt RD.key


del GSTRegistryGenerator.sis
del GSTRegistryGenerator.sisx
del GSTRegistryGenerator_update.sis
del GSTRegistryGenerator_update.sisx
del good_plugins_dlls.sis
del good_plugins_dlls.sisx