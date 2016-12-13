Imports System.Threading

Public Class Form1

    'Definitions
    Delegate Sub TextBoxDelegate(ByVal myStr As String)
    Dim Thread_0 As New Thread(AddressOf Thread_0_method) 'Initialization Thread
    Dim Thread_1 As New Thread(AddressOf Thread_1_method) 'Play Thread
    Dim Thread_2 As New Thread(AddressOf Thread_2_method) 'Pause Thread

    Private Sub Pause_Click(sender As Object, e As EventArgs) Handles Pause.Click
        'When button is clicked send an R to get the red response
        If SerialPort1.IsOpen Then
            SerialPort1.Write("I", 0, 1) 'Send the pause command to the AFP
        End If
    End Sub

    Private Sub Play_Click(sender As Object, e As EventArgs) Handles Play.Click
        'When button is clicked send a G to get the green response
        If SerialPort1.IsOpen Then
            SerialPort1.Write("P", 0, 1) 'Send the play command to the AFP
        End If

    End Sub

    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        'Trying the connection to the Serial Port
        Try
            SerialPort1.Open()
        Catch
            Console.WriteLine("Failed to open serial port")
        End Try
        Thread_0.Start()
    End Sub
    'Initialization Thread
    Private Sub Thread_0_method()
        SerialPort1.Write("N") 'Send initialization command to the AFP
        While SerialPort1.BytesToRead < 0
            'Getting the file names that the AFP sends during initialization
            IndexOfSongs.Items.Add(SerialPort1.ReadLine())
        End While
        ' Sort songs alphabetically
        IndexOfSongs.Sorted = True
    End Sub
    'The Play Thread
    Private Sub Thread_1_method()
        Dim str As String
        While SerialPort1.BytesToRead < 0

        End While
    End Sub
    'The Pause Thread
    Private Sub Thread_2_method()
        Dim str As String
        While SerialPort1.BytesToRead < 0

        End While
    End Sub
End Class