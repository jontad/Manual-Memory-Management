#Deviations/Avvikelser

#Implementation i inlupp 2
Programmet har inte implementerats i inlupp 2 helt och hållet. Detta för att vi inte hade tillräckligt med tid att fixa de sista buggarna, bland annat med frontend och resize för hashtabellen. Hade vi hunnit fixa buggarna så skulle vi haft en fullt fungerande implementation.

#Bitmap
Bitmap var en frivillig optimering vi nästan blev färdiga med. Vi lyckades få fram en något buggig version, där vår bitmap behövde vara väldigt stor för att inte skriva över existerande data. Möjligtvis går detta att fixa med en annan hashfunktion.

#(Overhead)
I nuläget har vi för mycket overhead för små allokeringar(8 bytes). Varje allokering tar 26 bytes extra. Om vi dock använder större allokeringar håller vi oss inom ramarna. Vid testerna för inlupp 2 med och utan vår implementation så fick vi en overhead på 1,66 x B, vilket också är inom ramarna. Så vi vet inte om detta är en avvikelse eller inte.
Vi lade fokus på att få ett fungerande program i första hand och prioriterade inte optimeringar. För att sänka overhead så skulle vi kunnat använda en bitmap för att använda mindre minne när vi sparar allokeringar.