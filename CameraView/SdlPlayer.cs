using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CameraView
{
    class SdlPlayer
    {
        [DllImport("Player.dll")]
        public static extern int SdlOpen(int handler, int width, int height);

        [DllImport("Player.dll")]
        public static extern void SdlClose();

        [DllImport("Player.dll")]
        public static extern void SdlTimer();
    }
}
