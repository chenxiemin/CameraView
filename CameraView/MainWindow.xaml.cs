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
        private string mplayUrl = "rtsp://192.168.0.128/12 rtsp://192.168.0.129/12 rtsp://192.168.0.130/12 rtsp://192.168.0.131/12";
        private string mrecordingFolder = "./video/";
        // private string mplayUrl = "rtsp://192.168.0.11/live-high";
        // private string mplayUrl = "rtsp://127.0.0.1/test.264 rtsp://127.0.0.1/test10.264";
        public MainWindow()
        {
            InitializeComponent();

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
                try {
                    string fileName = mrecordingFolder + "/" + index + "/";
                    fileName = System.IO.Path.Combine(System.IO.Directory.GetCurrentDirectory(), fileName);
                    System.IO.Directory.CreateDirectory(fileName);
                    fileName = System.IO.Path.Combine(fileName, DateTime.Now.ToString("yyyy-MM-dd-HH-mm-ss"));
                    SdlPlayer.SdlRecording(new StringBuilder(fileName), int.Parse(index), 1000);
                    tb.Foreground = new SolidColorBrush(Colors.Red);
                } catch(Exception e1) {
                    MessageBox.Show("Cannot recording: " + e1);
                }
            }
        }

        private void OnTabSelectionChanged(object sender, EventArgs args)
        {
            if (!recordingTabItem.IsSelected)
                return;

            recordingTree.Items.Clear();
            AddRecording(recordingTree.Items, new System.IO.DirectoryInfo(System.IO.Path.Combine(
                System.IO.Directory.GetCurrentDirectory(), mrecordingFolder)));
        }

        private void AddRecording(ItemCollection item, System.IO.DirectoryInfo directory)
        {
            try {
                System.IO.FileSystemInfo[] subInfos = directory.GetFileSystemInfos();
                foreach (System.IO.FileSystemInfo info in subInfos) {
                    if (info is System.IO.FileInfo) {
                        if (info.Extension == ".mp4") {
                            TreeViewItem fileTreeViewItem = new TreeViewItem();
                            item.Add(fileTreeViewItem);

                            fileTreeViewItem.Header = (info as System.IO.FileInfo).Name;
                            fileTreeViewItem.Tag = info;
                            fileTreeViewItem.MouseDoubleClick += OnTreeViewItemDoubleClick;
                        }
                    } else if (info is System.IO.DirectoryInfo) {
                        TreeViewItem subTreeViewItem = new TreeViewItem();
                        item.Add(subTreeViewItem);

                        subTreeViewItem.Header = info;
                        AddRecording(subTreeViewItem.Items, (info as System.IO.DirectoryInfo));
                    }
                }
            } catch (Exception e) {
                Console.WriteLine("Add recording error: " + e);
            }
        }

        private void OnTreeViewItemDoubleClick(object sender, EventArgs args)
        {
            TreeViewItem item = sender as TreeViewItem;
            System.IO.FileInfo info = item.Tag as System.IO.FileInfo;

            System.Diagnostics.Process.Start(info.FullName);
        }
    }
}
