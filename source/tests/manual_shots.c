/* The manual's screens, from the firmware itself on the computer (1.1.02): each view set directly (page, view,
 * window), no button-by-button route to keep in step with the menus. The working screens in RIBBON CLASSIC (its
 * orange palette), the themes' pictures in their own theme. Writes build/shots/NAME.raw (240 x 240 RGB565);
 * tests/manual_shots.py turns them into ../docs/img/NAME.png at 480 x 480.
 *   cc -O1 -DKAWA_WASM -DAPP_VER='"x"' -Ifirmware/src -Ifirmware/gen -Iweb tests/manual_shots.c -o build/host/shots */
#include "ksub.c"
#include <stdio.h>
#include <stdlib.h>
static void st(int n) { for (int i = 0; i < n; i++) { ksub_audio(); ksub_step(); } }
static void shot(const char *name)
{
    char p[96];
    g_force = 1; st(40);
    snprintf(p, sizeof p, "build/shots/%s.raw", name);
    FILE *f = fopen(p, "wb");
    if (!f) { perror(p); exit(1); }
    fwrite(fb, 2, 240 * 240, f); fclose(f);
}
static void look(uint32_t t, uint32_t pal) { set.theme = (uint8_t)t; set.pal = (uint8_t)pal; theme_set(set.theme, set.pal); font_set(set.font, set.zoom); }
static void goview(uint32_t vi) { uint32_t pg = 0; while (pg + 1u < NPAGE && PAGE_V0[pg + 1u] <= vi) pg++; page = (uint8_t)pg; vcur[pg] = (uint8_t)(vi - PAGE_V0[pg]); c3_show[pg] = 0; win = 0; }
static uint32_t vnamed(const char *cat) { for (uint32_t j = 0; j < sizeof VIEWS / sizeof VIEWS[0]; j++) if (str_eq(VIEWS[j].cat, cat)) return j; return 0; }
static void keys(uint32_t m) { ksub_keys(m); st(20); }
static void play(int on) { if (on != (int)sq_play) { seq_play(on); st(10); } }
static uint32_t dkp(const char *n) { for (uint32_t i = 0; i < sizeof DKP / sizeof DKP[0]; i++) if (str_eq(DKP[i].name, n)) return i + 1u; return 1; }

int main(void)
{
    int bt[3] = {3, 0, 3};
    ksub_init(); st(900);
    for (int j = 0; j < 3; j++) { ksub_button(bt[j], 1); st(2); ksub_button(bt[j], 0); st(40); }   /* (the first start's two questions) */
    st(400);                                    /* (their toasts gone) */

    look(6, 0);                                 /* RIBBON CLASSIC, EMBER (orange) */
    goview(0); keys(1u << 12 | 1u << 14); st(60); shot("p-home"); keys(0); st(150);
    goview(vnamed("OSCILLATOR A")); shot("rbc-osc");
    goview(vnamed("FILTER ENVELOPE")); shot("rbc-env");
    goview(V_SEQ); item_set(G_SQ_PRESET, 1); play(1); st(500); shot("d-seq"); play(0); st(100);
    goview(V_DRUM); dk_load_preset(dkp("TECHNO")); dk_psel = (uint8_t)dkp("TECHNO"); play(1); st(500); shot("d-drums");
    play(0); st(60); keys(1u << PK_WHITE[4]); { int i; for (i = 0; i < 6; i++) { ksub_turn(3, 2); st(4); } } shot("d-lock"); keys(0); st(60);
    item_set(G_DK_KIT, 2); dk_load_preset(dkp("DUB TECHNO")); dk_psel = (uint8_t)dkp("DUB TECHNO"); play(1); st(500); shot("d-drums-dub"); play(0); item_set(G_DK_KIT, 0); st(100);
    goview(V_SMP); item_set(SM_SEL, 7); patch[SM_START] = 40; patch[SM_END] = 90; keys(1u << 12); st(200); shot("v-sample-cut"); keys(0); patch[SM_SEL] = 0; st(100);
    goview(0); win = 4; mx_open(); st(10); ksub_turn(2, 40); ksub_turn(3, -30); st(40); shot("w-morph-xy"); win = 0; st(30);
    goview(0); win = 6; st(10); keys(1u << 2 | 1u << 12); st(60); shot("w-pads"); keys(0); win = 0; st(60);
    set.ui = 1; goview(vnamed("OSCILLATOR A")); c3_show[PG_BASS] = 1; shot("s-osc"); set.ui = 0; c3_show[PG_BASS] = 0;
    goview(PAGE_V0[PG_FILT]); shot("x-sel-filter");
    goview(V_SEQ); win = 8; shot("x-piano-roll"); win = 0;
    save_open(); shot("x-save");
    goview(PAGE_V0[PG_PRE]); shot("x-presets");

    look(5, 0); goview(vnamed("OSCILLATOR A")); st(200); shot("rb-osc");   /* RIBBON */
    look(0, 0); goview(vnamed("LFO 1 \xB7 WOBBLE")); shot("g-lfo");        /* CLASSIC GLOW */
    look(4, 0); goview(PAGE_V0[PG_PRE]); shot("k-presets");                /* TECH */
    {                                           /* the themes: the same screen (OSC A) in each */
        static const struct { uint8_t t, p; const char *n; } TH[] = {
            {7, 0, "t-classic"}, {0, 0, "t-glow-green"}, {0, 11, "t-glow-sky"}, {1, 0, "t-studio"}, {1, 2, "t-synthwave"}, {1, 5, "t-inverted"},
            {1, 8, "t-fluo"}, {1, 11, "t-sky"}, {1, 12, "t-blaze"}, {1, 19, "t-rad-red"}, {2, 0, "t-pure"}, {2, 1, "t-purered"}, {2, 22, "t-rad-pink"},
            {3, 0, "t-rack"}, {3, 1, "t-magma"}, {4, 0, "t-tech-volt"}, {4, 1, "t-tech-cyber"}, {5, 1, "t-ribbon-ice"}, {6, 0, "t-ribbon-classic"}};
        for (uint32_t i = 0; i < sizeof TH / sizeof TH[0]; i++) { look(TH[i].t, TH[i].p); goview(vnamed("OSCILLATOR A")); st(120); shot(TH[i].n); }
    }
    puts("shots done");
    return 0;
}
