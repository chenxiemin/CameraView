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
using System.Text.RegularExpressions;

namespace CameraView
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        // private string mplayUrl = "rtsp://192.168.0.128/12 rtsp://192.168.0.129/12 rtsp://192.168.0.130/12 rtsp://192.168.0.131/12";
        private string mplayUrl = "rtsp://192.168.0.11/live-high";
        public MainWindow()
        {
            InitializeComponent();

            // this.SourceInitialized += this.OnSourceInitialized;
            this.Loaded += this.OnLoaded;
        }

        void OnLoaded(object sender, EventArgs args)
        {
            // init player
            double width = this.contentGrid.RenderSize.Width / 5 * 4 - 5;
            double height = this.contentGrid.RenderSize.Height;
            NativeHost host = new NativeHost(width, height);
            host.PlayUrl = mplayUrl;
            Grid.SetRow(host, 0);
            Grid.SetColumn(host, 2);
            this.contentGrid.Children.Add(host);

            // get player count
            MatchCollection matches = Regex.Matches(mplayUrl, @"((2[0-4]\d|25[0-5]|[01]?\d\d?)\.){3}(2[0-4]\d|25[0-5]|[01]?\d\d?)");
            int i = 0;
            foreach (Match match in matches)
            {
                ListBoxItem item = new ListBoxItem();
                this.deviceList.Items.Add(item);

                item.Content = match.ToString();
                item.Tag = i++;
                item.MouseDoubleClick += this.OnListItemDoubleClick;
            }
        }

        private void OnModeButtonClick(object sender, EventArgs args)
        {
            SdlPlayer.SdlIterDisplayGrid();
        }

        private void OnPageLeftButtonClick(object sender, EventArgs args)
        {
            SdlPlayer.SdlPageLeft();
        }

        private void OnPageRightButtonClick(object sender, EventArgs args)
        {
            SdlPlayer.SdlPageRight();
        }

        private void OnExitMenuItemClick(object sender, EventArgs args)
        {
            this.Close();
        }

        private void OnListItemDoubleClick(object sender, MouseButtonEventArgs e)
        {
            SdlPlayer.SdlRecording(new StringBuilder("test.mp4"),
                int.Parse((sender as ListBoxItem).Tag.ToString()), 1000);
        }
    }
}
