/* Copyright 2012-2014 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.multimc.onesix;

import org.multimc.*;

import java.applet.Applet;
import java.io.File;
import java.awt.*;
import java.io.IOException;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

public class OneSixLauncher implements Launcher
{
	// parameters, separated from ParamBucket
	private List<String> libraries;
	private List<String> extlibs;
	private List<String> mcparams;
	private List<String> mods;
	private List<String> jarmods;
	private List<String> coremods;
	private List<String> traits;
	private String mainClass;
	private String natives;
	private String userName, sessionId;
	private String windowTitle;
	private String windowParams;

	// secondary parameters
	private String cwd;

	// the much abused system classloader, for convenience (for further abuse)
	private ClassLoader cl;

	private void processParams(ParamBucket params) throws NotFoundException
	{
		libraries = params.all("cp");
		extlibs = params.all("ext");
		mcparams = params.allSafe("param", new ArrayList<String>() );
		mainClass = params.firstSafe("mainClass", "net.minecraft.client.Minecraft");
		mods = params.allSafe("mod", new ArrayList<String>());
		jarmods = params.allSafe("jarmod", new ArrayList<String>());
		coremods = params.allSafe("coremod", new ArrayList<String>());
		traits = params.allSafe("traits", new ArrayList<String>());
		natives = params.first("natives");

		userName = params.first("userName");
		sessionId = params.first("sessionId");

		cwd = System.getProperty("user.dir");
	}

	private void printStats()
	{
		Utils.log("Main Class:");
		Utils.log("  " + mainClass);
		Utils.log();

		Utils.log("Native path:");
		Utils.log("  " + natives);
		Utils.log();

		Utils.log("Traits:");
		Utils.log("  " + traits);
		Utils.log();

		Utils.log("Libraries:");
		for (String s : libraries)
		{
			File f = new File(s);
			if (f.exists())
			{
				Utils.log("  " + s);
			}
			else
			{
				Utils.log("  " + s + " (missing)", "Warning");
			}
		}
		Utils.log();

		if(mods.size() > 0)
		{
			Utils.log("Mods:");
			for (String s : mods)
			{
				Utils.log("  " + s);
			}
			Utils.log();
		}

		if(coremods.size() > 0)
		{
			Utils.log("Core Mods:");
			for (String s : coremods)
			{
				Utils.log("  " + s);
			}
			Utils.log();
		}

		if(jarmods.size() > 0)
		{
			Utils.log("Jar Mods:");
			for (String s : jarmods)
			{
				Utils.log("  " + s);
			}
			Utils.log();
		}

		Utils.log("Params:");
		Utils.log("  " + mcparams.toString());
		Utils.log();
	}

	int launchWithMainClass()
	{
		// Get the Minecraft Class.
		Class<?> mc;
		try
		{
			mc = cl.loadClass(mainClass);
		} catch (ClassNotFoundException e)
		{
			System.err.println("Failed to find Minecraft main class:");
			e.printStackTrace(System.err);
			return -1;
		}

		// get the main method.
		Method meth;
		try
		{
			meth = mc.getMethod("main", String[].class);
		} catch (NoSuchMethodException e)
		{
			System.err.println("Failed to acquire the main method:");
			e.printStackTrace(System.err);
			return -1;
		}
		/*
		final java.nio.ByteBuffer[] icons = IconLoader.load("icon.png");
		new Thread() {
			public void run() {
				ClassLoader cl = ClassLoader.getSystemClassLoader();
				try
				{
					Class<?> Display;
					Method isCreated;
					Method setTitle;
					Method setIcon;
					Field fieldWindowCreated;
					Boolean created = false;
					Display = cl.loadClass("org.lwjgl.opengl.Display");
					fieldWindowCreated = Display.getDeclaredField("window_created");
					fieldWindowCreated.setAccessible( true );
					setTitle = Display.getMethod("setTitle", String.class);
					setIcon = Display.getMethod("setIcon", java.nio.ByteBuffer[].class);
					created = (Boolean) fieldWindowCreated.get( null );
					// set the window title? Maybe?
					while(!created)
					{
						try
						{
							Thread.sleep(150);
							created = (Boolean) fieldWindowCreated.get( null );
						} catch (InterruptedException ignored) {}
					}
					// Give it a bit more time ;)
					Thread.sleep(150);
					// set the title
					setTitle.invoke(null,windowTitle);
					// only set icon when there's actually something to set...
					if(icons.length > 0)
					{
						setIcon.invoke(null,(Object)icons);
					}
				}
				catch (Exception e)
				{
					System.err.println("Couldn't set window icon or title.");
					e.printStackTrace(System.err);
				}
			}
		}
		.start();
		*/
		// init params for the main method to chomp on.
		String[] paramsArray = mcparams.toArray(new String[mcparams.size()]);
		try
		{
			// static method doesn't have an instance
			meth.invoke(null, (Object) paramsArray);
		} catch (Exception e)
		{
			System.err.println("Failed to start Minecraft:");
			e.printStackTrace(System.err);
			return -1;
		}
		return 0;
	}

	@Override
	public int launch(ParamBucket params)
	{
		// get and process the launch script params
		try
		{
			processParams(params);
		} catch (NotFoundException e)
		{
			System.err.println("Not enough arguments.");
			e.printStackTrace(System.err);
			return -1;
		}

		// add libraries to classpath
		if(!Utils.addToClassPath(libraries))
		{
			System.err.println("Halting launch due to previous errors.");
			return -1;
		}

		// print the pretty things
		printStats();

		// extract native libs (depending on platform here... java!)
		Utils.log("Preparing native libraries...");
		String property = System.getProperty("os.arch");
		boolean is_64 = property.equalsIgnoreCase("x86_64") || property.equalsIgnoreCase("amd64");
		for(String extlib: extlibs)
		{
			try
			{
				String cleanlib = extlib.replace("${arch}", is_64 ? "64" : "32");
				File cleanlibf = new File(cleanlib);
				Utils.log("Extracting " + cleanlibf.getName());
				Utils.unzipNatives(cleanlibf, new File(natives));
			} catch (IOException e)
			{
				System.err.println("Failed to extract native library:");
				e.printStackTrace(System.err);
				return -1;
			}
		}
		Utils.log();

		// set the native libs path... the brute force way
		try
		{
			System.setProperty("java.library.path", natives);
			System.setProperty("org.lwjgl.librarypath", natives);
			System.setProperty("net.java.games.input.librarypath", natives);
			// by the power of reflection, initialize native libs again. DIRTY!
			// this is SO BAD. imagine doing that to ld
			Field fieldSysPath = ClassLoader.class.getDeclaredField("sys_paths");
			fieldSysPath.setAccessible( true );
			fieldSysPath.set( null, null );
		} catch (Exception e)
		{
			System.err.println("Failed to set the native library path:");
			e.printStackTrace(System.err);
			return -1;
		}

		// grab the system classloader and ...
		cl = ClassLoader.getSystemClassLoader();

		// normal launch just calls main()
		return launchWithMainClass();
	}
}
