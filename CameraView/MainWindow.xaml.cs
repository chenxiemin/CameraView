using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Interop;
using System.Threading;

namespace CameraView
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public static bool isStart = false;
        public MainWindow()
        {
            InitializeComponent();

            // this.SourceInitialized += this.OnSourceInitialized;
        }

        void OnSourceInitialized(object sender, EventArgs args)
        {
            /*
            WindowInteropHelper helper = new WindowInteropHelper(this);
            */

            // int hwnd = Player.Add(0, 800, 600);

            // NativeHost nativeHost = new NativeHost();
            // this.Content = nativeHost;
        }
    }
}
