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
    public partial class MainWindow : Window, PlayerCallback
    {
        // private string mplayUrl = "rtsp://192.168.0.128/12 rtsp://192.168.0.129/12 rtsp://192.168.0.130/12 rtsp://192.168.0.131/12";
        private string mplayUrl = "rtsp://192.168.0.11/live-high";
        // private string mplayUrl = "rtsp://127.0.0.1/test.264 rtsp://127.0.0.1/test10.264";
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
            host.Callback = this;
            host.PlayUrl = mplayUrl;
            Grid.SetRow(host, 0);
            Grid.SetColumn(host, 2);
            this.contentGrid.Children.Add(host);
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

        public void OnPlayerCallback(PlayerEvent playerEvnet, object args)
        {
            // init device list
            int playerCount = SdlPlayer.SdlGetPlayerCount();
            Player []players = new Player[playerCount];
            playerCount = SdlPlayer.SdlGetPlayer(players, playerCount);
            for (int i = 0; i < playerCount; i++)
            {
                ListBoxItem item = new ListBoxItem();
                deviceList.Items.Add(item);

                TextBlock tb = new TextBlock();
                tb.Text = players[i].name;

                item.Content = tb;
                item.Tag = players[i].id;
                item.MouseDoubleClick += this.OnListItemDoubleClick;
            }
        }

        private void OnListItemDoubleClick(object sender, MouseButtonEventArgs e)
        {
            TextBlock tb = (sender as ListBoxItem).Content as TextBlock;
            string index = (sender as ListBoxItem).Tag.ToString();
            if (SdlPlayer.SdlIsRecording(int.Parse(index)) > 0)
            {
                SdlPlayer.SdlStopRecording(int.Parse(index));
                tb.Foreground = new SolidColorBrush(Colors.Black);
            }
            else
            {
                SdlPlayer.SdlRecording(new StringBuilder(index), int.Parse(index), 1000);
                tb.Foreground = new SolidColorBrush(Colors.Red);
            }
        }
    }
}
