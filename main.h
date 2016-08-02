	
    sgTextRenderer tr;
	tr.SetResources(&g_DX11resources, g_DXmanager.m_pDeviceContext);
    tr.SetFontName("fonts/serveroff italic.fnt", "fonts/serveroff italic.png");
    tr.SetFontName("fonts/serveroff regular.fnt", "fonts/serveroff regular.png");
	tr.SetFontName("fonts/BadScript regular.fnt", "fonts/BadScript regular.png");
	tr.SetFontName("fonts/Margot.fnt", "fonts/Margot.png");

	tr.CreateFonts();

    tr.SetText(L"Cъешь ещЄ этих м€гких французских булок, да выпей же чаю.", 300, 10, BADSCRIPT_REGULAR, 0.5f);
	tr.SetColor(BADSCRIPT_REGULAR, 1.f, 1.f, 0.8f);
	tr.SetText(L"The quick brown fox jumps over the lazy dog.", 520, 500, MARGOT, 0.5);
	tr.SetText(L"The quick brown fox jumps over  the lazy dog.", 210, 500, 120, 200/*150*/, SERVEROFF_REGULAR, 0.4);
	tr.SetColor(SERVEROFF_REGULAR, 1.f, 0.f, 0.f/* 1.f, 0.f, 0.f*/);

	tr.EnableRender();


        tr.Render();
