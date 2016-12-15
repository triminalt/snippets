/**
 * @date 2014-02-28
 * @author triminalt AT gmail DOT com
 * @version initial
 *
 * ---
 *
 * @date 2016-12-12
 * @author triminalt AT gmail DOT com
 * @version v0.1
 *  Optimize all the interfaces and implementations.
 */

package cc.t34.java.utils;

import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.Objects;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;

public final class ReflectUtils {
    private static final String TAG = ReflectUtils.class.getSimpleName();

    private ReflectUtils() {
        // Disable.
    }
    
    public static final class Argument {
        public final Class<?> type;
        public final Object argument;
        
        /**
         * Constructor for a Argument.
         *
         * @param type, the initial type.
         * @param argument, the initial argument.
         */
        public Argument(Class<?> type, Object argument) {
            this.type = type;
            this.argument = argument;
        }

        /**
         * Checks the two Argument for equality by delegating to their respective
         * {@link Argument#equals(Object)} methods.
         *
         * @param obj the {@link Argument} to which this one is to be checked for equality
         * @return true if the underlying objects of the Argument are both considered equal.
         */
        @Override
        public boolean equals(Object obj) {
            if (!(obj instanceof Argument)) {
                return false;
            }
            Argument arg = (Argument) obj;
            return (Objects.equals(arg.type, type)) && Objects.equals(arg.argument, argument);
        }

        /**
         * Compute a hash code using the hash codes of the underlying objects.
         *
         * @return The hashcode of the Argument.
         */
        @Override
        public int hashCode() {
            return    (null == type ? 0 : type.hashCode())
                    ^ (null == argument ? 0 : argument.hashCode());
        }
    }

    public static final class ReturnValue {
        public static final ReturnValue FAILED = new ReturnValue(false, null);
        
        public final boolean code;
        public final Object value;

        /**
         * Constructor for a ReturnValue.
         *
         * @param code, the initial code.
         * @param value, the initial value.
         */
        public ReturnValue(boolean code, Object value) {
            this.code = code;
            this.value = value;
        }

        /**
         * Checks the two objects for equality by delegating to their respective
         * {@link ReturnValue#equals(Object)} methods.
         *
         * @param obj the {@link ReturnValue} to which this one is to be checked for equality
         * @return true if the underlying objects of the ReturnValue are both considered equal.
         */
        @Override
        public boolean equals(Object obj) {
            if (!(obj instanceof ReturnValue)) {
                return false;
            }
            ReturnValue val = (ReturnValue) obj;
            return (val.code == code) && Objects.equals(val.value, value);
        }

        /**
         * Compute a hash code using the hash codes of the underlying objects
         *
         * @return The hashcode of the ReturnValue.
         */
        @Override
        public int hashCode() {
            return Boolean.valueOf(code).hashCode() ^ (null == value ? 0 : value.hashCode());
        }

        public static ReturnValue succeeded(Object value) {
            return new ReturnValue(true, value);
        }
    }
    
    /**
     * Get a public property of a specified object.
     *
     * @param obj The specified object.
     * @param prop The name of the property.
     * @return The value of the property.
     */
    public static ReturnValue getPublicProperty(@Nonnull Object obj, @Nonnull String prop) {
        if (prop.isEmpty()) {
            throw new IllegalArgumentException(
                    TAG + ", [getPublicProperty]" + ", prop.isEmpty() ...");
        }

        final Field field = getPublicField(obj.getClass(), prop);
        if (null == field) {
            return ReturnValue.FAILED;
        }

        try {
            return ReturnValue.succeeded(field.get(obj));
        }
        catch (Throwable ignored) {
            return ReturnValue.FAILED;
        }
    }

    /**
     * Set a public property of a specified object.
     *
     * @param obj The specified object.
     * @param prop The name of the property.
     * @param value The new value of the property.
     * @return The old value of the property.
     */
    public static ReturnValue setPublicProperty(
            @Nonnull Object obj, @Nonnull String prop, @Nullable Object value) {
        if (prop.isEmpty()) {
            throw new IllegalArgumentException(
                    TAG + ", [setPublicProperty]" + ", prop.isEmpty() ...");
        }

        final Field field = getPublicField(obj.getClass(), prop);
        if (null == field) {
            return ReturnValue.FAILED;
        }

        try {
            final Object old = field.get(obj);
            field.set(obj, value);
            return ReturnValue.succeeded(old);
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return ReturnValue.FAILED;
        }
    }

    /**
     * Get a public static property of a specified class.
     *
     * @param clazz The class name of the specified class.
     * @param prop The name of the property.
     * @return The old value of the property.
     */
    public static ReturnValue getPublicProperty(@Nonnull String clazz, @Nonnull String prop) {
        if (clazz.isEmpty() || prop.isEmpty()) {
            throw new IllegalArgumentException(
                    TAG + ", [getPublicProperty]" + ", clazz.isEmpty() || prop.isEmpty() ...");
        }

        final Class<?> obj;
        try {
            obj = Class.forName(clazz);
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return ReturnValue.FAILED;
        }

        return getPublicProperty(obj, prop);
    }

    /**
     * Get a public static property of a specified class.
     *
     * @param clazz The class name of the specified class.
     * @param prop The name of the property.
     * @param value The new value of the property.
     * @return The old value of the property.
     */
    public static ReturnValue setPublicProperty(String clazz, String prop, Object value) {
        if (clazz.isEmpty() || prop.isEmpty()) {
            throw new IllegalArgumentException(
                    TAG + ", [setPublicProperty]" + ", clazz.isEmpty() || prop.isEmpty() ...");
        }

        final Class<?> obj;
        try {
            obj = Class.forName(clazz);
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return ReturnValue.FAILED;
        }

        return setPublicProperty(obj, prop, value);
    }

    /**
     * Get a public static property of a specified class.
     * @param clazz The specified class.
     * @param prop The name of the property.
     * @return The value of the property.
     */
    public static ReturnValue getPublicProperty(@Nonnull Class<?> clazz, @Nonnull String prop) {
        if (prop.isEmpty()) {
            throw new IllegalArgumentException(
                    TAG + ", [getPublicProperty]" + ", prop.isEmpty() ...");
        }

        final Field field = getPublicField(clazz, prop);
        if (null == field) {
            return ReturnValue.FAILED;
        }

        try {
            return ReturnValue.succeeded(field.get(clazz));
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return ReturnValue.FAILED;
        }
    }

    /**
     * Set a public static property of a specified class.
     *
     * @param clazz The specified class.
     * @param prop The name of the property.
     * @param value The new value of the property.
     * @return The old value of the property.
     */
    public static ReturnValue setPublicProperty(
            @Nonnull Class<?> clazz, @Nonnull String prop, @Nullable Object value) {
        if (prop.isEmpty()) {
            throw new IllegalArgumentException(
                    TAG + ", [setPublicProperty]" + ", prop.isEmpty() ...");
        }

        final Field field = getPublicField(clazz, prop);
        if (null == field) {
            return ReturnValue.FAILED;
        }

        try {
            final Object old = field.get(clazz);
            field.set(clazz, value);
            return ReturnValue.succeeded(old);

        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return ReturnValue.FAILED;
        }
    }

    /**
     * Get a property of a specified object.
     *
     * @param obj The specified object.
     * @param prop The name of the property.
     * @return The value of the property.
     */
    public static ReturnValue getProperty(@Nonnull Object obj, @Nonnull String prop) {
        if (prop.isEmpty()) {
            throw new IllegalArgumentException(
                    TAG + ", [getProperty]" + ", prop.isEmpty() ...");
        }

        final Field field = getField(obj.getClass(), prop);
        if (null == field) {
            return ReturnValue.FAILED;
        }

        try {
            final boolean accessible = field.isAccessible();
            field.setAccessible(true);
            final Object value = field.get(obj);
            field.setAccessible(accessible);
            return ReturnValue.succeeded(value);
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return ReturnValue.FAILED;
        }
    }

    /**
     * Get a static property of a specified class.
     *
     * @param clazz The name of the specified class.
     * @param prop The name of the property.
     * @return The value of the property.
     */
    public static ReturnValue getProperty(@Nonnull String clazz , @Nonnull String prop) {
        if (clazz.isEmpty() || prop.isEmpty()) {
            throw new IllegalArgumentException(
                    TAG + ", [getProperty]" + ", clazz.isEmpty() || property.isEmpty() ...");
        }

        try {
            return getProperty(Class.forName(clazz), prop);
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return ReturnValue.FAILED;
        }
    }

    /**
     * Get a static property of a specified class.
     *
     * @param clazz The specified class.
     * @param prop The name of the property.
     * @return The value of the property.
     */
    public static ReturnValue getProperty(@Nonnull Class<?> clazz, @Nonnull String prop) {
        if (prop.isEmpty()) {
            throw new IllegalArgumentException(
                    TAG + ", [getProperty]" + ", prop.isEmpty() ...");
        }

        @Nonnull

        final Field field = getField(clazz, prop);
        if (null == field) {
            return ReturnValue.FAILED;
        }

        try {
            final boolean accessible = field.isAccessible();
            field.setAccessible(true);
            final Object value = field.get(clazz);
            field.setAccessible(accessible);
            return ReturnValue.succeeded(value);
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return ReturnValue.FAILED;
        }
    }

    /**
     * Set a value the a property of a specified class.
     *
     * @param clazz The specified class.
     * @param prop The name of the property.
     * @return The value of the property.
     */
    public static ReturnValue setProperty(
            @Nonnull Class<?> clazz, @Nonnull String prop, @Nullable Object value) {
        if (prop.isEmpty()) {
            throw new IllegalArgumentException(
                    TAG + ", [setProperty]" + ", prop.isEmpty() ...");
        }

        final Field field = getField(clazz, prop);
        if (null == field) {
            return ReturnValue.FAILED;
        }

        try {
            final boolean accessible = field.isAccessible();
            field.setAccessible(true);
            final Object old = field.get(clazz);
            field.setAccessible(accessible);
            return ReturnValue.succeeded(old);
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return ReturnValue.FAILED;
        }
    }

    /**
     * Invoke a public method of a special object.
     *
     * @param obj The specified object.
     * @param name The name of the method.
     * @param args The arguments of the method.
     * @return The return value return by invoking the method.
     */
    public static ReturnValue invokePublicMethod(
            @Nonnull Object obj, String name, Argument[] args) {
        final Method method = getPublicMethod(obj.getClass(), name, args);
        if (null == method) {
            return ReturnValue.FAILED;
        }
        try {
            return ReturnValue.succeeded(method.invoke(obj, getArgumentValues(args)));
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return ReturnValue.FAILED;
        }
    }

    /**
     * Invoke a public static method of a special class.
     *
     * @param clazz The name of the specified class.
     * @param name The name of the method.
     * @param args The arguments of the method.
     * @return The return value return by invoking the method.
     */
    public static ReturnValue invokePublicMethod(
            @Nonnull String clazz, @Nonnull String name, @Nullable Argument[] args) {
        if (clazz.isEmpty() || name.isEmpty()) {
            throw new IllegalArgumentException(
                    TAG + ", [invokePublicMethod], clazz.isEmpty() || name.isEmpty() ...");
        }

        try {
            return invokePublicMethod(Class.forName(clazz), name, args);
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return ReturnValue.FAILED;
        }
    }

    /**
     * Invoke a public static method of a specified class.
     *
     * @param clazz The specified class.
     * @param name The name of the method.
     * @param args The arguments of the method.
     * @return The return value return by invoking the method.
     */
    public static ReturnValue invokePublicMethod(
            @Nonnull Class<?> clazz, @Nonnull String name, @Nullable Argument[] args) {
        if (name.isEmpty()) {
            throw new IllegalArgumentException(
                    TAG + ", [invokePublicMethod]" + ", name.isEmpty() ...");
        }

        final Method method = getPublicMethod(clazz, name, args);
        if (null == method) {
            return ReturnValue.FAILED;
        }

        try {
            return ReturnValue.succeeded(method.invoke(null, getArgumentValues(args)));
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return ReturnValue.FAILED;
        }
    }

    /**
     * Invoke a method of a specified object.
     *
     * @param obj The specified object.
     * @param name The name of the method.
     * @param args The arguments of the method.
     * @return The return value return by invoking the method.
     */
    public static
    @Nonnull ReturnValue invokeMethod(
            @Nonnull Object obj, @Nonnull String name, @Nullable Argument[] args) {
        if (name.isEmpty()) {
            throw new IllegalArgumentException(
                    TAG + ", [invokeMethod]" + ", name.isEmpty() ...");
        }

        Method method = getMethod(obj.getClass(), name, args);
        if (null == method) {
            return ReturnValue.FAILED;
        }

        try {
            final boolean accessible = method.isAccessible();
            method.setAccessible(true);
            Object result = method.invoke(obj, getArgumentValues(args));
            method.setAccessible(accessible);
            return ReturnValue.succeeded(result);
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return ReturnValue.FAILED;
        }
    }

    /**
     * Invoke a static method of a specified class.
     *
     * @param clazz The specified class.
     * @param name The name of the method.
     * @param args The arguments of the method.
     * @return The return value return by invoking the method.
     */
    public static
    @Nonnull ReturnValue invokeMethod(
            @Nonnull String clazz, @Nonnull String name, @Nullable Argument[] args) {
        try {
            return ReturnValue.succeeded(invokeMethod(Class.forName(clazz), name, args));
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return ReturnValue.FAILED;
        }
    }

    /**
     * Invoke a method of the specified object.
     *
     * @param clazz The specified class.
     * @param name The name of the method.
     * @param args The arguments of the method.
     * @return The return value return by invoking the method.
     */
    public static @Nonnull ReturnValue invokeMethod(
            @Nonnull Class<?> clazz, @Nonnull String name, @Nullable Argument[] args) {
        if (name.isEmpty()) {
            throw new IllegalArgumentException(
                    TAG + ", [invokeMethod]" + ", name.isEmpty() ...");
        }

        Method method = getMethod(clazz, name, args);
        if (null == method) {
            return ReturnValue.FAILED;
        }

        try {
            final boolean accessible = method.isAccessible();
            method.setAccessible(true);
            Object result = method.invoke(null, getArgumentValues(args));
            method.setAccessible(accessible);
            return ReturnValue.succeeded(result);
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return ReturnValue.FAILED;
        }
    }
    
    /**
     * Instantiate a new instance of the special class.
     *
     * @param clazz The name of the class
     * @param args The arguments of the constructor.
     * @return The instance of the class.
     */
    public static @Nonnull ReturnValue newInstance(
            @Nonnull String clazz, @Nullable Argument[] args) {
        if (clazz.isEmpty()) {
            throw new IllegalArgumentException(
                    TAG + ", [newInstance]" + ", clazz.isEmpty() ...");
        }

        try {
            return newInstance(Class.forName(clazz), args);
        }
        catch (ClassNotFoundException ex) {
            ex.printStackTrace();
            return ReturnValue.FAILED;
        }
    }

    /**
     * Instantiate a new instance of the special class.
     *
     * @param clazz The name of the class
     * @param args The arguments of the constructor.
     * @return The instance of the class.
     */
    public static @Nonnull ReturnValue newInstance(
            @Nonnull Class<?> clazz, @Nullable Argument[] args) {
        final Constructor<?> constructor = getConstructor(clazz, args);
        if (null == constructor) {
            return ReturnValue.FAILED;
        }

        try {
            return ReturnValue.succeeded(constructor.newInstance(getArgumentValues(args)));
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return ReturnValue.FAILED;
        }
    }

    /**
     * Determine whether the object is a instance of the specified class.
     *
     * @param clazz The specified class.
     * @param obj The instance to be determine.
     * @return Return true if the object is the instance of the specified class, otherwise, false.
     */
    public static boolean isInstance(@Nonnull String clazz, Object obj) {
        if (clazz.isEmpty()) {
            throw new IllegalArgumentException(
                    TAG + ", [isInstance]" + ", clazz.isEmpty() ...");
        }

        try {
            return Class.forName(clazz).isInstance(obj);
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return false;
        }
    }
    
    private static Class<?>[] getParameterTypes(Argument ... args) {
        Class<?>[] clazzs = null;
        if (null != args && 0 != args.length) {
            clazzs = new Class[args.length];
            for (int i = 0; i < args.length; ++i) {
                clazzs[i] = args[i].type;
            }
        }
        return clazzs;
    }
    
    private static Object[] getArgumentValues(Argument ... args) {
        Object[] values = null;
        if (null != args && 0 != args.length) {
            values = new Object[args.length];
            for (int i = 0; i < args.length; ++i) {
                values[i] = args[i].argument;
            }
        }
        return values;
    }

    private static Field getPublicField(@Nonnull Class<?> clazz, String name) {
        try {
            return clazz.getField(name);
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return null;
        }
    }

    private static Field getField(Class<?> clazz, String name) {
        try {
            return clazz.getField(name);
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
        }

        try {
            return clazz.getDeclaredField(name);
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return null;
        }
    }

    private static Method getPublicMethod(Class<?> clazz, String name, Argument[] args) {
        return getPublicMethod(clazz, name, getParameterTypes(args));
    }

    private static Method getPublicMethod(@Nonnull Class<?> clazz, String name, Class<?>[] args) {
        try {
            return clazz.getMethod(name, args);
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return null;
        }
    }

    private static Method getMethod(Class<?> clazz, String name, Argument[] args) {
        return getMethod(clazz, name, getParameterTypes(args));
    }

    private static Method getMethod(Class<?> clazz, String name, Class<?>[] args) {
        try {
            return clazz.getMethod(name, args);
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
        }

        try {
            return clazz.getDeclaredMethod(name, args);
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return null;
        }
    }

    private static Constructor<?> getConstructor(Class<?> clazz, Argument[] args) {
        return getConstructor(clazz, getParameterTypes(args));
    }

    private static Constructor<?> getConstructor(@Nonnull Class<?> clazz, Class<?>[] args) {
        try {
            return clazz.getConstructor(args);
        }
        catch (Throwable ignored) {
            ignored.printStackTrace(System.err);
            return null;
        }
    }
}
