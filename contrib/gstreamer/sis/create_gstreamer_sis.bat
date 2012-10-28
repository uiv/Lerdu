makesis   .\GSTRegistryGenerator.pkg   .\GSTRegistryGenerator.sis
signsis   .\GSTRegistryGenerator.sis   .\GSTRegistryGenerator.sisx RD.crt RD.key

makesis   .\gstreamer_framework.pkg   .\gstreamer_framework.sis
signsis   .\gstreamer_framework.sis   .\gstreamer_framework.sisx RD.crt RD.key

makesis   .\gstreamer.pkg   .\gstreamer.sis
signsis   .\gstreamer.sis   .\gstreamer.sis RD.crt RD.key

del GSTRegistryGenerator.sis
del GSTRegistryGenerator.sisx
del gstreamer_framework.sis
del gstreamer_framework.sisx