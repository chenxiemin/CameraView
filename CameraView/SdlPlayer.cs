using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CameraView
{
    [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Ansi)]
    struct Player
    {
        public int id;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
        public string name;
    }

    class SdlPlayer
    {
        [DllImport("Player.dll")]
        public static extern int SdlOpen(IntPtr handler, int width, int height, StringBuilder url);

        [DllImport("Player.dll")]
        public static extern void SdlClose();

        [DllImport("Player.dll")]
        public static extern void SdlTimer();

        [DllImport("Player.dll")]
        public static extern int SdlGetPlayerCount();

        [DllImport("Player.dll")]
        public static extern int SdlGetPlayer([In, Out] Player[] players, int arraySize);

        [DllImport("Player.dll")]
        public static extern void SdlIterDisplayGrid();

        [DllImport("Player.dll")]
        public static extern void SdlPageLeft();

        [DllImport("Player.dll")]
        public static extern void SdlPageRight();

        [DllImport("Player.dll")]
        public static extern int SdlRecording(StringBuilder name, int channel, int time);

        [DllImport("Player.dll")]
        public static extern void SdlStopRecording(int channel);

        [DllImport("Player.dll")]
        public static extern int SdlIsRecording(int channel);

        [DllImport("Player.dll")]
        public static extern void SdlMouseEvent(int type, int button, int x, int y);

        public const int SDL_EVENT_MOUSE_BUTTON_DOWN = 0x401;
        public const int SDL_MOUSE_BUTTON_RIGHT = 3;
    }
}
