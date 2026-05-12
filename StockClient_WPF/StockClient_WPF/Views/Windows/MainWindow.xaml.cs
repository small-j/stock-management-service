using Google.Protobuf;
using Microsoft.Extensions.DependencyInjection;
using StockClient_WPF.Interfaces;
using StockClient_WPF.ViewModels.Windows;
using StockClient_WPF.Views.Pages;
using StockServiceProtocol;
using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace StockClient_WPF.Views.Windows
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindowViewModel ViewModel { get; }

        public MainWindow(MainWindowViewModel viewModel)
        {
            ViewModel = viewModel;

            InitializeComponent();

            var mainPage = App.Services.GetRequiredService<Main>();
            MainFrame.Navigate(mainPage);
        }
    }
}
