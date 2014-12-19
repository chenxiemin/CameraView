using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Win32ControlInWpfWindow
{
    class NativeMethods
    {
        // 
        // Helper methods
        //
        internal static uint GetHiword(int doubleWord)
        {
            return ((uint)doubleWord >> 16 & 0xFFFF);
        }

        internal static uint GetLoword(int doubleWord)
        {
            return ((uint)doubleWord & 0xFFFF);
        }


        //
        // DLL Imports
        //
        [DllImport("user32.dll", EntryPoint = "CreateWindowEx", CharSet = CharSet.Auto)]
        internal static extern IntPtr CreateWindowEx(
            int exStyle,
            string className,
            string windowName,
            int style,
            int x, int y,
            int width, int height,
            IntPtr hwndParent,
            IntPtr hMenu,
            IntPtr hInstance,
            [MarshalAs(UnmanagedType.AsAny)] object pvParam);

        [DllImport("user32.dll", EntryPoint = "DestroyWindow", CharSet = CharSet.Auto)]
        internal static extern bool DestroyWindow(
            IntPtr hwnd);

        [DllImport("user32.dll", EntryPoint = "GetFocus", CharSet = CharSet.Auto)]
        internal static extern IntPtr GetFocus();

        [DllImport("user32.dll", EntryPoint = "SendMessage", CharSet = CharSet.Auto)]
        internal static extern int SendMessage(
            IntPtr hwnd, int message, IntPtr wParam, IntPtr lParam);

        [DllImport("user32.dll", EntryPoint = "SendMessage", CharSet = CharSet.Auto)]
        internal static extern int SendMessage(
            IntPtr hwnd, int message, int wParam, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder lParam);

        [DllImport("user32.dll", EntryPoint = "SendMessage", CharSet = CharSet.Auto)]
        internal static extern IntPtr SendMessage(
            IntPtr hwnd, int message, IntPtr wParam, String lParam);

        [DllImport("user32.dll", EntryPoint = "SetFocus", CharSet = CharSet.Auto)]
        internal static extern IntPtr SetFocus(
            IntPtr hwnd);

        [DllImport("user32.dll", EntryPoint = "SetWindowPos", CharSet = CharSet.Auto)]
        internal static extern bool SetWindowPos(
            IntPtr hwnd, IntPtr hwndInsertAfter, int x, int y, uint width, uint height, uint flags);

        [DllImport("user32.dll", EntryPoint = "MoveWindow", CharSet = CharSet.Auto)]
        internal static extern bool MoveWindow(
            IntPtr hwnd, int x, int y, uint width, uint height, bool brepaint);

        //
        // Win32 window styles
        //
        internal const int WS_CHILD = 0x40000000;
        internal const int WS_VISIBLE = 0x10000000;
        internal const int WS_VSCROLL = 0x00200000;
        internal const int WS_BORDER = 0x00800000;

        //
        // Win32 Listbox Messages
        //
        internal const int LBN_SELCHANGE = 0x001;
        internal const int LB_SETCURSEL = 0x186;
        internal const int LB_GETCURSEL = 0x188;
        internal const int LB_GETTEXTLEN = 0x18A;
        internal const int LB_ADDSTRING = 0x180;
        internal const int LB_GETTEXT = 0x189;
        internal const int LB_DELETESTRING = 0x182;
        internal const int LB_GETCOUNT = 0x18B;
        internal const int LBS_NOTIFY = 0x001;

        //
        // Win32 messages
        //
        internal const int WM_CREATE = 0x001;
        internal const int WM_MOVE = 0x003;
        internal const int WM_SIZE = 0x005;
        internal const int WM_QUIT = 0x0012;
        internal const int WM_COMMAND = 0x111;
        internal const int WM_KEYDOWN = 0x100;
        internal const int WM_LBUTTONDOWN = 0x201;
        internal const int WM_LBUTTONUP = 0x202;
        internal const int WM_RBUTTONDOWN = 0x204;
        internal const int WM_NCHITTEST = 0x84;

        // 
        // SetWindowPos flags
        //
        internal const uint SWP_NOMOVE = 0x0002;
        internal const uint SWP_NOZORDER = 0x0004;
        internal const uint SWP_NOACTIVATE = 0x0010;
        internal const uint SWP_SHOWWINDOW = 0x0040;
        internal const uint SWP_NOOWNERZORDER = 0x0200;

        //
        // Virtual key codes
        //
        internal const int VK_TAB = 0x09;
        internal const int VK_SHIFT = 0x10;
        internal const int VK_UP = 0x26;
        internal const int VK_DOWN = 0x28;
    }
}
