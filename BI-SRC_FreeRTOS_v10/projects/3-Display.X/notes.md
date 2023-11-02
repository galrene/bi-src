### 2
- Dejte jednomu z vypisujících procesů vyšší prioritu (ale stále nižší než je priorita gatekeeperu), pozorujte frekvenci výpisu jednotlivých zpráv
---
Odpoved:
- spusta sa iba task s tou vyssou prioritou,
pretoze vzdy ked by sa mal spustat ten s nizsou, uz uplynul timeout tomu s vyssou a teda sa spusti ten?

### 3
- Vytvořte funkci vDisplayPrintTask2, bude identická s vDisplayPrintTask, ale k čekání bude používat funkci vTaskDelay
- vytvořte jeden proces z funkce vDisplayPrintTask s nižší prioritou a jeden z vDisplayPrintTask2 s vyšší prioritou
- pozorujte frekvenci výpisu jednotlivých zpráv
---
Odpoved:
- teraz sa spusta aj ten s nizsou prioritou, pretoze kvoli vTaskDelay main proces zaspi, uvolni planovanie a zatial sa spusti ten s nizsou prioritou?

### 4
- Původní funkci vDisplayPrintTask nahraďe vDisplayPrintTask2 vytvořte z funkce vDisplayPrintTask 2 procesy a experimentujte s prioritami
---
Odpoved:
- ked ma jedno nizsiu prioritu ako druhe, spusta ju sa obidva ale ten s vyssou prioritou vzdy prvy
- ked maju rovnake priority, spustaju sa obidva, v poradi v akom boli vytvorene
