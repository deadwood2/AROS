using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;

[assembly: AssemblyTitle("AROS Wizard powered by ChatGPT")]
[assembly: AssemblyProduct("AROS Wizard powered by ChatGPT")]

internal static class ArosSetup
{
    private static int Main(string[] args)
    {
        if (args.Length == 1 && args[0] == "--self-test")
        {
            using (Stream script = Assembly.GetExecutingAssembly().GetManifestResourceStream("AROS.ps1"))
            {
                if (script == null || script.Length == 0 || !Environment.Is64BitProcess) return 1;
                Console.WriteLine("AROS Wizard powered by ChatGPT: embedded PowerShell script verified.");
                return 0;
            }
        }

        string options = "";
        bool shareWindowsFiles = false;
        bool writableShare = false;
        foreach (string arg in args)
        {
            switch (arg)
            {
                case "--update": options += " -Update"; break;
                case "--auto-update": options += " -AutoUpdate"; break;
                case "--cleanup": options += " -CleanupOnly"; break;
                case "--offline-iso": options += " -OfflineIso"; break;
                case "--distributions": options += " -ChooseDistribution"; break;
                case "--chatgpt": options += " -ChatGPT"; break;
                case "--virtual-drives": options += " -VirtualDrives"; break;
                case "--smp": options += " -Smp"; break;
                case "--driver-debug": options += " -DriverDebug"; break;
                case "--wanderer-fullscreen": options += " -WandererFullscreen"; break;
                case "--windows-desktop": options += " -WindowsDesktop"; break;
                case "--share": options += " -ShareWindowsFiles"; shareWindowsFiles = true; break;
                case "--share-write": writableShare = true; break;
                default:
                    Console.Error.WriteLine("Usage: AROS-Setup-x64.exe [--update | --auto-update | --cleanup | --offline-iso | --distributions | --share | --share-write | --windows-desktop | --chatgpt | --wanderer-fullscreen | --virtual-drives | --smp | --driver-debug]");
                    return 2;
            }
        }
        if (writableShare)
        {
            if (!shareWindowsFiles) options += " -ShareWindowsFiles";
            options += " -WritableShare";
        }

        string temporaryScript = Path.Combine(Path.GetTempPath(), "aros-" + Guid.NewGuid().ToString("N") + ".ps1");
        try
        {
            using (Stream script = Assembly.GetExecutingAssembly().GetManifestResourceStream("AROS.ps1"))
            using (FileStream output = new FileStream(temporaryScript, FileMode.CreateNew))
            {
                script.CopyTo(output);
            }

            ProcessStartInfo start = new ProcessStartInfo(
                Path.Combine(Environment.SystemDirectory, @"WindowsPowerShell\v1.0\powershell.exe"),
                "-NoProfile -Sta -ExecutionPolicy Bypass -File \"" + temporaryScript + "\"" + options);
            start.UseShellExecute = false;
            start.EnvironmentVariables["AROS_SETUP_DIRECTORY"] = AppDomain.CurrentDomain.BaseDirectory;
            using (Process process = Process.Start(start))
            {
                process.WaitForExit();
                if (process.ExitCode == 0) return 0;
                Console.Error.WriteLine("AROS Wizard powered by ChatGPT endete mit Fehlercode " + process.ExitCode + ".");
            }
        }
        catch (Exception error)
        {
            Console.Error.WriteLine(error.Message);
        }
        finally
        {
            try { File.Delete(temporaryScript); }
            catch (IOException) { }
            catch (UnauthorizedAccessException) { }
        }

        if (!Console.IsInputRedirected)
        {
            Console.WriteLine("Zum Schliessen Enter druecken.");
            Console.ReadLine();
        }
        return 1;
    }
}

