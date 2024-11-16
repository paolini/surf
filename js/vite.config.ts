import { defineConfig } from 'vite';

export default defineConfig({
  build: {
    outDir: 'dist', // Output directory for the frontend build
    assetsDir: 'assets', // Folder for static assets like JS, CSS, images
  },
});