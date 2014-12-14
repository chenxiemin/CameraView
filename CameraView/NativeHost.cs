using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Interop;
using System.Windows.Threading;
using Win32ControlInWpfWindow;

namespace CameraView
{
    public enum PlayerEvent
    {
        PLAYED
    }

    public interface PlayerCallback
    {
        void OnPlayerCallback(PlayerEvent playerEvnet, object args);
    }

    class NativeHost : HwndHost
    {
        private int mwidth = 0;
        private int mheight = 0;
        private IntPtr handler;
        private DispatcherTimer mtimer;
        private bool misInit = false;
        private string mplayUrl;
        private PlayerCallback mcallback = null;

        public PlayerCallback Callback
        {
            get { return mcallback; }
            set { mcallback = value; }
        }

        public string PlayUrl
        {
            get { return mplayUrl; }
            set { mplayUrl = value; }
        }

        public NativeHost(double width, double height)
        {
            mwidth = (int)width;
            mheight = (int)height;
        }

        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            this.handler = NativeMethods.CreateWindowEx(
                0, "static", "",
                NativeMethods.WS_CHILD | NativeMethods.WS_VISIBLE | NativeMethods.LBS_NOTIFY,
                0, 0, mwidth, mheight, hwndParent.Handle, IntPtr.Zero, IntPtr.Zero, 0);

            return new HandleRef(this, handler);
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            if (null != this.mtimer) {
                this.mtimer.Stop();
                SdlPlayer.SdlClose();
            }

            NativeMethods.DestroyWindow(hwnd.Handle);
        }

        protected override IntPtr WndProc(IntPtr hwnd, int message, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            handled = false;

            switch (message)
            {
                case NativeMethods.WM_SIZE:
                    if (!misInit)
                        misInit = true;
                    else
                        break;
                    SdlPlayer.SdlOpen(handler, mwidth, mheight, new StringBuilder(PlayUrl));
                    if (null != Callback)
                        Callback.OnPlayerCallback(PlayerEvent.PLAYED, null);

                    this.mtimer = new DispatcherTimer();
                    this.mtimer.Interval = TimeSpan.FromMilliseconds(0);
                    this.mtimer.Tick += this.OnTimer;
                    this.mtimer.Start();

                    break;
                case NativeMethods.WM_LBUTTONUP:
                    break;
            }

            return IntPtr.Zero;
        }

        private void OnTimer(object sender, EventArgs args)
        {
            SdlPlayer.SdlTimer();
        }
    }
}
