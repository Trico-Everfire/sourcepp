using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

namespace vpkpp
{
    using EntryCallback = Action<string, Entry>;

    using EntryPredicate = Func<string, Entry, bool>;

    using EntryCreation = Func<string, EntryOptions>;

    internal unsafe delegate void EntryCallbackNative(string path, void* entry);

    internal unsafe delegate int EntryPredicateNative(string path, void* entry);

    internal unsafe delegate EntryOptions EntryCreationNative(string path);

    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

        [DllImport("vpkppc")]
        public static extern StringArray vpkpp_get_openable_extensions();

        [DllImport("vpkppc")]
        public static extern PackFileType vpkpp_get_type(void* handle);

        [DllImport("vpkppc")]
        public static extern int vpkpp_has_entry_checksums(void* handle);

        [DllImport("vpkppc")]
        public static extern StringArray vpkpp_verify_entry_checksums(void* handle);

        [DllImport("vpkppc")]
        public static extern int vpkpp_has_pack_file_checksum(void* handle);

        [DllImport("vpkppc")]
        public static extern int vpkpp_verify_pack_file_checksum(void* handle);

        [DllImport("vpkppc")]
        public static extern int vpkpp_has_pack_file_signature(void* handle);

        [DllImport("vpkppc")]
        public static extern int vpkpp_verify_pack_file_signature(void* handle);

        [DllImport("vpkppc")]
        public static extern int vpkpp_is_case_sensitive(void* handle);

        [DllImport("vpkppc")]
        public static extern int vpkpp_has_entry(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path, int includeUnbaked);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_find_entry(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path, int includeUnbaked);

        [DllImport("vpkppc")]
        public static extern Buffer vpkpp_read_entry(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern String vpkpp_read_entry_text(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern int vpkpp_is_read_only(void* handle);

        [DllImport("vpkppc")]
        public static extern void vpkpp_add_entry_from_file(void* handle, [MarshalAs(UnmanagedType.LPStr)] string entryPath, [MarshalAs(UnmanagedType.LPStr)] string filepath);

        [DllImport("vpkppc")]
        public static extern void vpkpp_add_entry_from_file_with_options(void* handle, [MarshalAs(UnmanagedType.LPStr)] string entryPath, [MarshalAs(UnmanagedType.LPStr)] string filepath, EntryOptions options);

        [DllImport("vpkppc")]
        public static extern void vpkpp_add_entry_from_mem(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path, byte* buffer, ulong bufferLen);

        [DllImport("vpkppc")]
        public static extern void vpkpp_add_entry_from_mem_with_options(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path, byte* buffer, ulong bufferLen, EntryOptions options);

        [DllImport("vpkppc")]
        public static extern void vpkpp_add_directory(void* handle, [MarshalAs(UnmanagedType.LPStr)] string entryBaseDir, [MarshalAs(UnmanagedType.LPStr)] string dir, IntPtr creation);

        [DllImport("vpkppc")]
        public static extern void vpkpp_add_directory_with_options(void* handle, [MarshalAs(UnmanagedType.LPStr)] string entryBaseDir, [MarshalAs(UnmanagedType.LPStr)] string dir, EntryOptions options);

        [DllImport("vpkppc")]
        public static extern int vpkpp_rename_entry(void* handle, [MarshalAs(UnmanagedType.LPStr)] string oldPath, [MarshalAs(UnmanagedType.LPStr)] string newPath);

        [DllImport("vpkppc")]
        public static extern int vpkpp_rename_directory(void* handle, [MarshalAs(UnmanagedType.LPStr)] string oldDir, [MarshalAs(UnmanagedType.LPStr)] string newDir);

        [DllImport("vpkppc")]
        public static extern int vpkpp_remove_entry(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern ulong vpkpp_remove_directory(void* handle, [MarshalAs(UnmanagedType.LPStr)] string dirName);

        [DllImport("vpkppc")]
        public static extern int vpkpp_bake(void* handle, [MarshalAs(UnmanagedType.LPStr)] string outputDir, IntPtr callback);

        [DllImport("vpkppc")]
        public static extern int vpkpp_bake_with_options(void* handle, [MarshalAs(UnmanagedType.LPStr)] string outputDir, BakeOptions options, IntPtr callback);

        [DllImport("vpkppc")]
        public static extern int vpkpp_extract_entry(void* handle, [MarshalAs(UnmanagedType.LPStr)] string entryPath, [MarshalAs(UnmanagedType.LPStr)] string filepath);

        [DllImport("vpkppc")]
        public static extern int vpkpp_extract_directory(void* handle, [MarshalAs(UnmanagedType.LPStr)] string dir, [MarshalAs(UnmanagedType.LPStr)] string outputDir);

        [DllImport("vpkppc")]
        public static extern int vpkpp_extract_all(void* handle, [MarshalAs(UnmanagedType.LPStr)] string outputDir, int createUnderPackFileDir);

        [DllImport("vpkppc")]
        public static extern int vpkpp_extract_all_if(void* handle, [MarshalAs(UnmanagedType.LPStr)] string outputDir, IntPtr predicate, int stripSharedDirs);

        [DllImport("vpkppc")]
        public static extern ulong vpkpp_get_entry_count(void* handle, int includeUnbaked);

        [DllImport("vpkppc")]
        public static extern void vpkpp_run_for_all_entries(void* handle, IntPtr operation, int includeUnbaked);

        [DllImport("vpkppc")]
        public static extern String vpkpp_get_filepath(void* handle);

        [DllImport("vpkppc")]
        public static extern String vpkpp_get_truncated_filepath(void* handle);

        [DllImport("vpkppc")]
        public static extern String vpkpp_get_filename(void* handle);

        [DllImport("vpkppc")]
        public static extern String vpkpp_get_truncated_filename(void* handle);

        [DllImport("vpkppc")]
        public static extern String vpkpp_get_filestem(void* handle);

        [DllImport("vpkppc")]
        public static extern String vpkpp_get_truncated_filestem(void* handle);

        [DllImport("vpkppc")]
        public static extern Attribute vpkpp_get_supported_entry_attributes(void* handle);

        [DllImport("vpkppc")]
        public static extern String vpkpp_to_string(void* handle);

        [DllImport("vpkppc")]
        public static extern void vpkpp_close(void** handle);

        [DllImport("vpkppc")]
        public static extern String vpkpp_escape_entry_path_for_write([MarshalAs(UnmanagedType.LPStr)] string path);
    }

    public class PackFile
    {
        private protected unsafe PackFile(void* handle)
        {
            Handle = handle;
        }

        ~PackFile()
        {
            unsafe
            {
                fixed (void** handlePtr = &Handle)
                {
                    Extern.vpkpp_close(handlePtr);
                }
            }
        }

        public static PackFile? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_open(path, 0);
                return handle == null ? null : new PackFile(handle);
            }
        }

        public static PackFile? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.vpkpp_open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new PackFile(handle);
            }
        }

        public static string EscapeEntryPathForWrite(string path)
        {
            var str = Extern.vpkpp_escape_entry_path_for_write(path);
            return StringUtils.ConvertToStringAndDelete(ref str);
        }

        public static List<string> GetSupportedFileTypes()
        {
            var stringArray = Extern.vpkpp_get_openable_extensions();
            return StringUtils.ConvertToListAndDelete(ref stringArray);
        }

        public bool HasEntryChecksums()
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_has_entry_checksums(Handle));
            }
        }

        public IEnumerable<string> VerifyEntryChecksums()
        {
            unsafe
            {
                var stringArray = Extern.vpkpp_verify_entry_checksums(Handle);
                return StringUtils.ConvertToListAndDelete(ref stringArray);
            }
        }

        public bool HasPackFileChecksum()
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_has_pack_file_checksum(Handle));
            }
        }

        public bool VerifyPackFileChecksum()
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_verify_pack_file_checksum(Handle));
            }
        }

        public bool HasPackFileSignature()
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_has_pack_file_signature(Handle));
            }
        }

        public bool VerifyPackFileSignature()
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_verify_pack_file_signature(Handle));
            }
        }

        public bool HasEntry(string path, bool includeUnbaked = true)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_has_entry(Handle, path, Convert.ToInt32(includeUnbaked)));
            }
        }

        public Entry? FindEntry(string path, bool includeUnbaked = true)
        {
            unsafe
            {
                var entry = Extern.vpkpp_find_entry(Handle, path, Convert.ToInt32(includeUnbaked));
                return entry == null ? null : new Entry(entry, false);
            }
        }

        public byte[]? ReadEntry(string path)
        {
            unsafe
            {
                var buffer = Extern.vpkpp_read_entry(Handle, path);
                return buffer.size < 0 ? null : BufferUtils.ConvertToArrayAndDelete(ref buffer);
            }
        }

        public string? ReadEntryText(string path)
        {
            unsafe
            {
                var str = Extern.vpkpp_read_entry_text(Handle, path);
                return str.size < 0 ? null : StringUtils.ConvertToStringAndDelete(ref str);
            }
        }

        public void AddEntry(string entryPath, string filepath)
        {
            unsafe
            {
                Extern.vpkpp_add_entry_from_file(Handle, entryPath, filepath);
            }
        }

        public void AddEntry(string entryPath, string filepath, EntryOptions options)
        {
            unsafe
            {
                Extern.vpkpp_add_entry_from_file_with_options(Handle, entryPath, filepath, options);
            }
        }

        public void AddEntry(string path, byte[] buffer)
        {
            unsafe
            {
                fixed (byte* bufferPtr = buffer)
                {
                    Extern.vpkpp_add_entry_from_mem(Handle, path, bufferPtr, (ulong) buffer.LongLength);
                }
            }
        }

        public void AddEntry(string path, byte[] buffer, EntryOptions options)
        {
            unsafe
            {
                fixed (byte* bufferPtr = buffer)
                {
                    Extern.vpkpp_add_entry_from_mem_with_options(Handle, path, bufferPtr, (ulong) buffer.LongLength, options);
                }
            }
        }

        public void AddEntry(string path, IEnumerable<byte> buffer)
        {
            unsafe
            {
                var data = buffer.ToArray();
                fixed (byte* bufferPtr = data)
                {
                    Extern.vpkpp_add_entry_from_mem(Handle, path, bufferPtr, (ulong)data.LongLength);
                }
            }
        }

        public void AddEntry(string path, IEnumerable<byte> buffer, EntryOptions options)
        {
            unsafe
            {
                var data = buffer.ToArray();
                fixed (byte* bufferPtr = data)
                {
                    Extern.vpkpp_add_entry_from_mem_with_options(Handle, path, bufferPtr, (ulong)data.LongLength, options);
                }
            }
        }

        public void AddDirectory(string entryBasePath, string dir)
        {
            unsafe
            {
                Extern.vpkpp_add_directory(Handle, entryBasePath, dir, 0);
            }
        }

        public void AddDirectory(string entryBasePath, string dir, EntryOptions options)
        {
            unsafe
            {
                Extern.vpkpp_add_directory_with_options(Handle, entryBasePath, dir, options);
            }
        }

        public void AddDirectory(string entryBasePath, string dir, EntryCreation creation)
        {
            unsafe
            {
                EntryCreationNative callbackNative = (path) =>
                {
                    return creation(path);
                };
                Extern.vpkpp_add_directory(Handle, entryBasePath, dir, Marshal.GetFunctionPointerForDelegate(callbackNative));
            }
        }

        public bool RenameEntry(string oldPath, string newPath)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_rename_entry(Handle, oldPath, newPath));
            }
        }

        public bool RenameDirectory(string oldDir, string newDir)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_rename_directory(Handle, oldDir, newDir));
            }
        }

        public bool RemoveEntry(string path)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_remove_entry(Handle, path));
            }
        }

        public ulong RemoveDirectory(string dirName)
        {
            unsafe
            {
                return Extern.vpkpp_remove_directory(Handle, dirName);
            }
        }

        public bool Bake(string outputDir)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_bake(Handle, outputDir, 0));
            }
        }

        public bool Bake(string outputDir, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                return Convert.ToBoolean(Extern.vpkpp_bake(Handle, outputDir, Marshal.GetFunctionPointerForDelegate(callbackNative)));
            }
        }

        public bool Bake(string outputDir, BakeOptions options)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_bake_with_options(Handle, outputDir, options, 0));
            }
        }

        public bool Bake(string outputDir, BakeOptions options, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                return Convert.ToBoolean(Extern.vpkpp_bake_with_options(Handle, outputDir, options, Marshal.GetFunctionPointerForDelegate(callbackNative)));
            }
        }

        public bool ExtractEntry(string entryPath, string filepath)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_extract_entry(Handle, entryPath, filepath));
            }
        }

        public bool ExtractDirectory(string dir, string outputDir)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_extract_directory(Handle, dir, outputDir));
            }
        }

        public bool ExtractAll(string outputDir, bool createUnderPackFileDir = true)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_extract_all(Handle, outputDir, Convert.ToInt32(createUnderPackFileDir)));
            }
        }

        public bool ExtractAll(string outputDir, EntryPredicate predicate, bool stripSharedDirs = true)
        {
            unsafe
            {
                EntryPredicateNative predicateNative = (path, entry) =>
                {
                    return Convert.ToInt32(predicate(path, new Entry(entry, true)));
                };
                return Convert.ToBoolean(Extern.vpkpp_extract_all_if(Handle, outputDir, Marshal.GetFunctionPointerForDelegate(predicateNative), Convert.ToInt32(stripSharedDirs)));
            }
        }

        public ulong GetEntryCount(bool includeUnbaked = true)
        {
            unsafe
            {
                return Extern.vpkpp_get_entry_count(Handle, Convert.ToInt32(includeUnbaked));
            }
        }

        public void RunForAllEntries(EntryCallback operation, bool includeUnbaked = true)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    operation(path, new Entry(entry, true));
                };
                Extern.vpkpp_run_for_all_entries(Handle, Marshal.GetFunctionPointerForDelegate(callbackNative), Convert.ToInt32(includeUnbaked));
            }
        }

        public override string ToString()
        {
            unsafe
            {
                var str = Extern.vpkpp_to_string(Handle);
                return StringUtils.ConvertToStringAndDelete(ref str);
            }
        }

        public PackFileType Type
        {
            get
            {
                unsafe
                {
                    return Extern.vpkpp_get_type(Handle);
                }
            }
        }

        public bool CaseSensitive
        {
            get
            {
                unsafe
                {
                    return Convert.ToBoolean(Extern.vpkpp_is_case_sensitive(Handle));
                }
            }
        }

        public bool ReadOnly
        {
            get
            {
                unsafe
                {
                    return Convert.ToBoolean(Extern.vpkpp_is_read_only(Handle));
                }
            }
        }

        public string FilePath
        {
            get
            {
                unsafe
                {
                    var str = Extern.vpkpp_get_filepath(Handle);
                    return StringUtils.ConvertToStringAndDelete(ref str);
                }
            }
        }

        public string TruncatedFilePath
        {
            get
            {
                unsafe
                {
                    var str = Extern.vpkpp_get_truncated_filepath(Handle);
                    return StringUtils.ConvertToStringAndDelete(ref str);
                }
            }
        }

        public string FileName
        {
            get
            {
                unsafe
                {
                    var str = Extern.vpkpp_get_filename(Handle);
                    return StringUtils.ConvertToStringAndDelete(ref str);
                }
            }
        }

        public string TruncatedFileName
        {
            get
            {
                unsafe
                {
                    var str = Extern.vpkpp_get_truncated_filename(Handle);
                    return StringUtils.ConvertToStringAndDelete(ref str);
                }
            }
        }

        public string FileStem
        {
            get
            {
                unsafe
                {
                    var str = Extern.vpkpp_get_filestem(Handle);
                    return StringUtils.ConvertToStringAndDelete(ref str);
                }
            }
        }

        public string TruncatedFileStem
        {
            get
            {
                unsafe
                {
                    var str = Extern.vpkpp_get_truncated_filestem(Handle);
                    return StringUtils.ConvertToStringAndDelete(ref str);
                }
            }
        }

        public Attribute SupportedEntryAttributes
        {
            get
            {
                unsafe
                {
                    return Extern.vpkpp_get_supported_entry_attributes(Handle);
                }
            }
        }

        private protected readonly unsafe void* Handle;
    }
}
