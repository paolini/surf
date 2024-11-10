import { defineConfig } from 'vite';

export default defineConfig({
  build: {
    target: 'node21', // Adjust based on your Node version
    lib: {
        entry: 'export.ts',
        formats: ['cjs'], // CommonJS format per Node.js
        fileName: () => 'export.js',
    },
    rollupOptions: {
        external: ['fs','path','os','three'], // no dependencies
    },
    outDir: 'dist/export',
  },
});
