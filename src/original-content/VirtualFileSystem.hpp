/**
 * \file
 *
 * This module implements a wrapper around ZenLibs Virtual Systen, or short VDFS.
 *
 * A VDFS can be constructed by loading package-files or mounting a real directory on
 * the host file-system. All files found in either the packages or the real directory
 * are then added to a VDFS-FileIndex.
 *
 * To get the data of a file, the FileIndex can be queried. It will resolve where the
 * real file is and load the data from it.
 *
 * See BsZenLib or ZenLib for more information.
 *
 *
 * # Differences to Gothics implementation
 *
 * While Gothics VDFS kept the directory structure inside the VDFS, it did not make use
 * of it at any point. Keeping the directory structure even led to problems with threading,
 * as multiple threads were setting the current working directory at the same time.
 * This caused the infamous "Orc-Axe"-Bug in the temple of the sleeper.
 *
 * To simplify things in REGoth, the directory structure is not preserved inside our
 * implementation of the VDFS. All files gathered into a single list.
 *
 *
 * # Design decisions
 *
 * As in the original and the previous iteration of REGoth, it was found that there
 * is no need to ever have multiple VDFS-FileIndices around. Therefore, the API to
 * the FileIndex REGoth uses acts on one global file index, which saves us from
 * passing the FileIndex-object to everything.
 *
 */

#pragma once

#include <FileSystem/BsPath.h>

namespace VDFS
{
  class FileIndex;
}

namespace REGoth
{
  class InternalVirtualFileSystem;
  class VirtualFileSystem
  {
  public:
    VirtualFileSystem() = default;
    virtual ~VirtualFileSystem() = default;

    /**
     * Before calling anything else, this function must be called.
     *
     * The PhysFS-Library used by our VDFS-implementation wants to know where the program
     * executable currently running is located.
     *
     * @param argv0 First value of the argv array passed to main (Path to executable).
     */
    void setPathToEngineExecutable(const bs::String& argv0);

    /**
     * Before the VDFS can load packages, it needs to know where gothics data files are,
     * which can be set using this function.
     *
     * @param gameDirectory  Path to gothics root directory
     */
    void setGameDirectory(const bs::Path& gameDirectory);

    /**
     * Checks gothics data-directory for a package with the given name (case-insensitive).
     *
     * @note Needs a game directory to work. See setGameDirectory().
     *
     * @param  package  Name of the package, with extension (eg. "Meshes.vdf").
     *
     * @return Whether the given package is available.
     */
    bool isPackageAvailable(const bs::String& package) const;

    /**
     * Loads a package into the global file index.
     *
     * The package will be loaded from the games data directory, if possible.
     * After loading, the files of the given package can be found inside the
     * file index and their data can be obtained using readFile().
     *
     * To check whether a package exists, call isPackageAvailable(). However,
     * loadPackage() *will* do the same check inside and return whether the
     * package exists.
     *
     * Also not that you *cannot* load more packages after you have read the
     * first file. Make sure to load all packages first.
     *
     * @note   Needs a game directory to work. See setGameDirectory().
     *
     * @param  package  Name of the package, with extension (eg. "Meshes.vdf").
     *
     * @return Whether the package could be loaded.
     */
    bool loadPackage(const bs::String& package);


    /**
     * Reads the contents of a file and returns them as byte array.
     *
     * To load a package, which contains files, see loadPackage().
     *
     * @note   Check with hasFile() whether the file exists within the file index.
     *
     * @param  file  Case-insensitive name of the file found inside a package previously loaded
     *               using loadPackage(), eg. "STONE.3DS".
     *
     * @return The complete data from the given file. Empty if the file does not exist.
     */
    bs::Vector<bs::UINT8> readFile(const bs::String& file) const;

    /**
     * Searches through the file index to see if the given file has been registered
     * inside the file index.
     *
     * See loadPackage() on how to populat the file index with files.
     *
     * @param  file  Case-insensitive name of the file found inside a package previously loaded
     *               using loadPackage(), eg. "STONE.3DS".
     *
     * @return Whether the file is available and can be read using readFile().
     */
    bool hasFile(const bs::String& file) const;


    /**
     * @return Whether (at least some) of the game files were found in the given data directory
     *         after loading packages.
     */
    bool hasFoundGameFiles() const;

    /**
     * Returns a reference to the internal file index for other modules to use (eg. BsZenLib).
     *
     * This will also cause the file index to be finalized, so no new packages can be loaded
     * afterwards.
     */
    const VDFS::FileIndex& getFileIndex();

  private:

    /**
     * Throws an exception if the mInternal-pointer is not set.
     */
    void throwOnMissingInternalState() const;

    bs::SPtr<InternalVirtualFileSystem> mInternal;
  };

  /**
   * Global access to the virtual file system.
   */
  VirtualFileSystem& gVirtualFileSystem();
}