using System;
using System.Windows.Input;
using System.Windows.Interop;
using System.Runtime.InteropServices;
using System.Text;

namespace Win32ControlInWpfWindow
{
    /// <summary>
    /// Hosts a Win32 Listbox control, demonstrating WPF-Win32 interop.
    /// Exposes a set of methods (AddItem, DeleteItem, etc.) allowing manipulation of the data in the hosted Win32 listbox.
    /// Implements the 3 obligatory methods declared by HwndHost (BuildWindowCore, DestroyWindowCore, WndProc). 
    /// </summary>
    class Win32ListBoxHost : HwndHost, IKeyboardInputSink
    {
        public void AddItem(string item)
        {
            if (item == String.Empty)
            {
                throw new ArgumentException("Invalid parameter. You cannot add an empty string to a list box.", "item");
            }

            NativeMethods.SendMessage(_hwndListBox, NativeMethods.LB_ADDSTRING, IntPtr.Zero, item);
        }

        public void DeleteItem(int itemIndex)
        {
            NativeMethods.SendMessage(_hwndListBox, NativeMethods.LB_DELETESTRING, (IntPtr)itemIndex, IntPtr.Zero);
            OnSelectionChanged(new EventArgs());
        }

        public int SelectedItemIndex
        {
            get
            {
                return NativeMethods.SendMessage(_hwndListBox, NativeMethods.LB_GETCURSEL, IntPtr.Zero, IntPtr.Zero);
            }
            set
            {
                NativeMethods.SendMessage(_hwndListBox, NativeMethods.LB_SETCURSEL, (IntPtr)value, IntPtr.Zero);
                OnSelectionChanged(new EventArgs());
            }
        }

        public string SelectedItemText
        {
            get
            {
                StringBuilder itemText = new StringBuilder();
                NativeMethods.SendMessage(_hwndListBox, NativeMethods.LB_GETTEXT, this.SelectedItemIndex, itemText);

                return itemText.ToString();
            }
        }

        public event EventHandler SelectionChanged;

        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            _hwndListBox = NativeMethods.CreateWindowEx(
                0, "listbox", "",
                NativeMethods.WS_CHILD | NativeMethods.WS_VISIBLE | NativeMethods.LBS_NOTIFY | NativeMethods.WS_VSCROLL | NativeMethods.WS_BORDER,
                0, 0,
                (int)Width, (int)Height,
                hwndParent.Handle,
                IntPtr.Zero,
                IntPtr.Zero,
                0);

            return new HandleRef(this, _hwndListBox);
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            NativeMethods.DestroyWindow(hwnd.Handle);
        }

        protected virtual void OnSelectionChanged(EventArgs args)
        {
            EventHandler handler = SelectionChanged;
            if (handler != null)
            {
                handler(this, args);
            }
        }

        /// <summary>
        /// This is the WindowProc of the hosted Win32 window. 
        /// The window proc allows you to handle messages, sent to the window. 
        /// </summary>
        /// <param name="hwnd">Win32 handle to the hosted Win32 window</param>
        /// <param name="message">Win32 message sent to the hosted Win32 window</param>
        /// <param name="wParam"></param>
        /// <param name="lParam"></param>
        /// <param name="handled">Set to true if the message is handled, false otherwise</param>
        /// <returns></returns>
        protected override IntPtr WndProc(IntPtr hwnd, int message, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            handled = false;

            switch (message)
            {
                //
                // The code below does the selection notification for the Win32 listbox
                // Note that the selection notification is done by handling "simple" ("raw") messages such as
                // WM_LBUTTONUP. If you prefer to handle "composite" messages such as WM_COMMAND, then 
                // you should host the Listbox HWND in another HWND as demonstrated in the sample in
                // http://msdn2.microsoft.com/en-us/library/ms752055.aspx
                //

                case NativeMethods.WM_LBUTTONUP:
                    OnSelectionChanged(new EventArgs());
                    break;
            }

            return IntPtr.Zero;
        }

        /// <summary>
        /// This method implements tabbing into the Win32 listbox (either by pressing Tab or by pressing
        /// Shift+Tab). 
        /// </summary>
        /// <param name="request">Used to determine the direction of the navigation ("Tab" vs "Shift-Tab")</param>
        /// <returns></returns>
        bool IKeyboardInputSink.TabInto(TraversalRequest request)
        {
            if (request.FocusNavigationDirection == FocusNavigationDirection.Next)
            {
                NativeMethods.SetFocus(_hwndListBox);
            }
            else
            {
                NativeMethods.SetFocus(_hwndListBox);
            }

            return true;
        }

        /// <summary>
        /// This method implements support for keyboard navigation inside the Win32 listbox
        /// using the "Up" and "Down" keys
        /// </summary>
        /// <returns>True if a message if handled, false otherwise</returns>
        bool IKeyboardInputSink.TranslateAccelerator(ref MSG msg, ModifierKeys modifiers)
        {
            bool isHandled = false;

            if (msg.message == NativeMethods.WM_KEYDOWN)
            {
                if (msg.wParam == (IntPtr)NativeMethods.VK_UP)
                {
                    if (this.SelectedItemIndex > 0)
                    {
                        this.SelectedItemIndex--;
                    }
                    else
                    {
                        this.SelectedItemIndex = 0;
                    }
                    isHandled = true;
                }
                if (msg.wParam == (IntPtr)NativeMethods.VK_DOWN)
                {
                    this.SelectedItemIndex++;
                    isHandled = true;
                }
            }

            return isHandled;
        }


        private IntPtr _hwndListBox = IntPtr.Zero;
    }
}
