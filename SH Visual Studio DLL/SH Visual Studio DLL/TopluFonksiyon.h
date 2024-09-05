void TopluFonksiyon() {

    while (true) {
        if (GetAsyncKeyState(VK_F1) != 0) {
            break;
        }
        else {
        }
    }

    bool PauseBilgi;
    int FareX, FareY;
    float ToplamX = 0, ToplamY = 0;
    float KameraX, KameraY, KameraZ;
    float KarakterX = 0, KarakterY = 0, KarakterZ = 0;
    float Yaricap = 140; //Radius
    float Hassasiyet = 0.5f; //Sensitivity

    POINT Fare1;

    HWND hwnd_SonicHeroesTM = FindWindowA(NULL, "SONIC HEROES(TM)");
    DWORD ProcessIDSonicHeroes = NULL;
    GetWindowThreadProcessId(hwnd_SonicHeroesTM, &ProcessIDSonicHeroes);
    HANDLE HandleSonicHeroes = NULL;
    HandleSonicHeroes = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessIDSonicHeroes);

    char SonicHeroesGameModule[] = "Tsonic_win.exe";
    DWORD SonicHeroesAnaAdress = GetModuleBaseAddress(_T(SonicHeroesGameModule), ProcessIDSonicHeroes);

    // Pause
    DWORD PauseAdresi = 0x000041F4;
    vector<DWORD> PauseOffseti{ 0x0 };
    DWORD KullanPause = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesAnaAdress, PauseAdresi, PauseOffseti);

    // X Karakter
    DWORD XKarakterAdresi = 0x005CE820;
    vector<DWORD> XKarakterOffseti{ 0xE8 };

    // Y Karakter
    DWORD YKarakterAdresi = 0x005CE820;
    vector<DWORD> YKarakterOffseti{ 0xEC };

    // Z Karakter
    DWORD ZKarakterAdresi = 0x005CE820;
    vector<DWORD> ZKarakterOffseti{ 0xF0 };

    GorevSilici(0x006207DB);
    GorevSilici(0x006207DC);

    GorevSilici(0x006207E3);
    GorevSilici(0x006207E4);
    GorevSilici(0x006207E5);

    GorevSilici(0x006207EC);
    GorevSilici(0x006207ED);
    GorevSilici(0x006207EE);

    while (true) {
        ReadProcessMemory(HandleSonicHeroes, (PBYTE*)KullanPause, &PauseBilgi, sizeof(PauseBilgi), 0);

        if (PauseBilgi != 1) {
        }

        else {

            GetCursorPos(&Fare1);

            FareX = Fare1.x;
            FareY = Fare1.y;

            Sleep(1);

            GetCursorPos(&Fare1);

            ToplamX += Hassasiyet * (FareX - Fare1.x) * 3.14f / 180;
            ToplamY += Hassasiyet * (Fare1.y - FareY) * 3.14f / 180;

            if (ToplamY >= 1) {
                ToplamY = 1;
            }

            else if (ToplamY <= -1) {
                ToplamY = -1;
            }

            else {

            }

            DWORD KullanXKarakter = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesAnaAdress, XKarakterAdresi, XKarakterOffseti);
            DWORD KullanYKarakter = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesAnaAdress, YKarakterAdresi, YKarakterOffseti);
            DWORD KullanZKarakter = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesAnaAdress, ZKarakterAdresi, ZKarakterOffseti);

            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)KullanXKarakter, &KarakterX, sizeof(KarakterX), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)KullanYKarakter, &KarakterY, sizeof(KarakterY), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)KullanZKarakter, &KarakterZ, sizeof(KarakterZ), 0);

            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A60C30, &KameraX, sizeof(KameraX), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A60C34, &KameraY, sizeof(KameraY), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A60C38, &KameraZ, sizeof(KameraZ), 0);

            KameraY = KarakterY + Yaricap * sin(ToplamY);
            KameraX = KarakterX + Yaricap * cos(ToplamY) * sin(ToplamX);
            KameraZ = KarakterZ + Yaricap * cos(ToplamY) * cos(ToplamX);

            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A60C30, &KameraX, sizeof(KameraX), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A60C34, &KameraY, sizeof(KameraY), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A60C38, &KameraZ, sizeof(KameraZ), 0);
        }
    }
}
