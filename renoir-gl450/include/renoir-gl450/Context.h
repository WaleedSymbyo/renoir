#pragma once

struct Renoir_Handle;

struct Renoir_GL450_Context;

struct Renoir_Settings;

Renoir_GL450_Context*
renoir_gl450_context_new(Renoir_Settings* settings);

void
renoir_gl450_context_free(Renoir_GL450_Context* self);

void
renoir_gl450_context_window_init(Renoir_GL450_Context* self, Renoir_Handle* h, Renoir_Settings* settings);

void
renoir_gl450_context_window_free(Renoir_GL450_Context* self, Renoir_Handle* h);

void
renoir_gl450_context_window_bind(Renoir_GL450_Context* self, Renoir_Handle* h);

void
renoir_gl450_context_bind(Renoir_GL450_Context* self);

void
renoir_gl450_context_unbind(Renoir_GL450_Context* self);

void
renoir_gl450_context_window_present(Renoir_GL450_Context* self, Renoir_Handle* h);
